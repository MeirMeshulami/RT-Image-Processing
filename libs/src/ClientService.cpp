#include "ClientService.h"
#include "Frame.h"
#include "JsonManager.h"



ClientService::ClientService() : stopStreaming(false), frameShowQueue(std::make_shared<ThreadSafeQueue<std::shared_ptr<Frame>>>()) {}

void ClientService::Connect(std::string serverAddress) {
	LOG_DEBUG("Starting frame receiver thread...");
	std::thread connectionThread([serverAddress, this] {
		LOG_INFO("Client connecting...");
		channel = grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
		stub_ = FrameService::NewStub(channel);
		if (!stub_)
			LOG_ERROR("Cannot create a channel !");
		else
			LOG_INFO("Channel has created successfully !");

		auto state = channel->GetState(true);
		int retries = 30;
		while (state != grpc_connectivity_state::GRPC_CHANNEL_READY && retries > 0) {
			LOG_INFO("Waiting for channel to be ready, current state: {}", state);

			channel->WaitForStateChange(state, std::chrono::system_clock::now() + std::chrono::seconds(10));
			state = channel->GetState(true);
			retries--;
		}

		if (state == grpc_connectivity_state::GRPC_CHANNEL_READY) {
			LOG_INFO("Client connected.");
			isConnect.store(true);
		}
		else {
			LOG_ERROR("Failed to connect to the server after several attempts.");
			isConnect.store(false);
		}
		});

	connectionThread.detach();
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
	if (frameShowQueue->GetQueueSize() > 30)
	{
		std::shared_ptr<Frame> toThrowOut;
		frameShowQueue->TryPop(toThrowOut);
		LOG_DEBUG("frameShowQueue size= {}", frameShowQueue->GetQueueSize());
		LOG_ERROR("The frameShowQueue reached its maximum capacity ({}) and popped out frame number {}.", frameShowQueue->GetQueueSize(), toThrowOut->GetFrameNum());
	}
	frameShowQueue->Push(frame);
}

std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> ClientService::GetFrameShowQueue() { return frameShowQueue; }

std::shared_ptr<Frame> ClientService::GetFrameToDetect() { return frameToDetect; }

void ClientService::DestroyConnection() {
	LOG_INFO("Disconnecting from camera...");
	isConnect.store(false);
	//ReleaseVideoWriter();
	LOG_INFO("Disconnected.");
}

void ClientService::StartStreamFrames() {
	std::thread streamThread([&] {
		stopStreaming.store(false);
		GetFrame();
		});
	streamThread.detach();
}

void ClientService::StopStreamFrames() {
	LOG_INFO("Stopping stream..");
	stopStreaming.store(true);
}
