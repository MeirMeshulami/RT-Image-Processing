#include "ClientService.h"
#include "Frame.h"
#include "JsonManager.h"



ClientService::ClientService(std::shared_ptr<grpc::Channel> channel)
	: stub_(FrameService::NewStub(channel)),
	isShowingFrames(true),
	stopStreaming(false)
{
	frameShowQueue = std::make_shared<ThreadSafeQueue<std::shared_ptr<Frame>>>();
	isFreeToDetect = std::make_shared<std::atomic<bool>>(true);
	if (!stub_)
		LOG_ERROR("Cannot create a channel !");
	else
		LOG_INFO("Channel has created successfully !");

}

void ClientService::GetFrame() {
	LOG_INFO("Getting images from camera...");
	FrameRequest request;
	FrameResponse response;

	grpc::ClientContext context;
	std::unique_ptr<grpc::ClientReader<FrameResponse>> reader(stub_->GetFrame(&context, request));

	if (!reader) {
		LOG_ERROR("Failed to create reader !");
		throw std::runtime_error("Failed to create reader !");
	}
	else {
		LOG_DEBUG("Reader has created successfully !");
	}

	while (reader->Read(&response)) {
		const auto& imageData = response.image_data();
		std::vector<uchar> buffer(imageData.begin(), imageData.end());
		cv::Mat receivedFrame = cv::imdecode(buffer, cv::IMREAD_COLOR);

		if (receivedFrame.empty()) {
			LOG_WARN("Recived an empty frame !");
		}
		else {
			auto resFrameTime = response.frame_time();
			auto resFrameNum = response.frame_number();
			auto frameTimePoint = std::chrono::system_clock::time_point(std::chrono::seconds(resFrameTime.seconds()) + std::chrono::milliseconds(resFrameTime.nanos() / 1000000));

			LOG_DEBUG("Received frame {} with timestamp {}", resFrameNum, frameTimePoint);
			auto currentFrame = std::make_shared<Frame>(receivedFrame, resFrameNum, frameTimePoint);
			PushFrameIntoQueue(currentFrame);
			LOG_INFO("Frame {} pushed into the show queues.", resFrameNum);
		}
		if (stopStreaming) {
			LOG_INFO("Reader has finished.");
			context.TryCancel();
			break;
		}
	}


	grpc::Status status = reader->Finish();
	if (!status.ok()) {
		LOG_ERROR("Error while getting frames: {} ", status.error_message());
	}
	return;
}

bool ClientService::UpdateConfigs(const std::string& file) {
	UpdateConfig request;
	ConfigAck ack;
	grpc::ClientContext context;

	request.set_file(file);
	LOG_DEBUG("Sending configs updates...");
	grpc::Status status = stub_->UpdateConfigurations(&context, request, &ack);

	if (status.ok()) {
		LOG_INFO("Config update has arrived.");
	}
	else {
		LOG_ERROR("Config send failed: {}", status.error_message());
	}

	return ack.success();
}

void ClientService::PushFrameIntoQueue(std::shared_ptr<Frame> frame) {
	if (isShowingFrames)
	{
		if (frameShowQueue->GetQueueSize() > 30)
		{
			std::shared_ptr<Frame> toThrowOut;
			frameShowQueue->TryPop(toThrowOut);
			LOG_DEBUG("frameShowQueue size= {}", frameShowQueue->GetQueueSize());
			LOG_ERROR("The frameShowQueue reached its maximum capacity ({}) and popped out frame number {}.", frameShowQueue->GetQueueSize(), toThrowOut->GetFrameNum());
		}
		frameShowQueue->Push(frame);
	}
}

std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> ClientService::GetFrameShowQueue() { return frameShowQueue; }

std::shared_ptr<Frame> ClientService::GetFrameToDetect() { return frameToDetect; }

std::shared_ptr<std::atomic<bool>> ClientService::IsFreeToDetect() { return isFreeToDetect; }

void ClientService::ControlShowingFrames() {
	isShowingFrames = !isShowingFrames;
	if (isShowingFrames)
		LOG_INFO("Showing video started.");
	else
		LOG_INFO("Showing video stoped.");
}

void ClientService::ControlProcessingFrames() {
	isProcessingFrames = !isProcessingFrames;
	if (isProcessingFrames)
		LOG_INFO("Processing frames started.");
	else
		LOG_INFO("Processing frames stoped.");
}



