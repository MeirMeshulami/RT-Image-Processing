#include "ServerService.h"

ServerService::ServerService() : cam(0) {
	if (!cam.isOpened()) {
		throw std::runtime_error("The camera is already open in another program !");
	}
}

grpc::Status ServerService::GetFrame(grpc::ServerContext* context, const FrameRequest* request, grpc::ServerWriter<FrameResponse>* writer) {
	LOG_INFO("Sending frames...");

	int32_t frameNum = 0;
	cv::Mat previousFrame;
	cam.read(previousFrame);

	while (cam.isOpened()) {
		configs.CheckIfJsonModified();
		auto& configJson = configs.configJson;

		cv::Mat currentFrame;
		if (!cam.read(currentFrame)) {
			LOG_INFO("Error: Failed to read a frame from the camera.");
			return grpc::Status(grpc::StatusCode::CANCELLED, "Error: Failed to read a frame from the camera.");
		}

		double threshold = configJson["camera_settings"]["threshold"];
		int maxDiffPixels = configJson["camera_settings"]["max_diff_pixels"];
		auto numMotionPixels = MotionDetector(currentFrame, previousFrame, threshold);

		if (numMotionPixels > maxDiffPixels) {
			FrameResponse response;
			response.set_frame_number(frameNum++);
			std::chrono::system_clock::time_point frameTime = std::chrono::system_clock::now();
			response.mutable_frame_time()->set_seconds(std::chrono::system_clock::to_time_t(frameTime));
			response.mutable_frame_time()->set_nanos(std::chrono::duration_cast<std::chrono::nanoseconds>(frameTime.time_since_epoch()).count() % 1000000000);

			std::vector<uchar> encodedImage;
			cv::imencode(".jpg", currentFrame, encodedImage);
			response.set_image_data(reinterpret_cast<const char*>(encodedImage.data()), encodedImage.size());
			LOG_DEBUG("frame {} has sended.", frameNum);

			if (!writer->Write(response)) {
				LOG_INFO("Client disconnected.");
				return grpc::Status(grpc::StatusCode::CANCELLED, "Client disconnected.");
			}
		}
		previousFrame = currentFrame;
	}

	return grpc::Status::OK;
}

void ServerService::RunServer() {

	auto& configJson = configs.configJson;
	std::string serverIp = configJson["grpc_settings"]["server_ip_address"];
	std::string port = configJson["grpc_settings"]["port_number"];
	std::string server_address(serverIp + ":" + port);

	grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(this);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::thread exitThread([&server] {
		while (true) {
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				server->Shutdown();
				return;
			}
		}
		});
	exitThread.detach();

	LOG_INFO("Server listening on {}", server_address);
	server->Wait();
}

grpc::Status ServerService::UpdateConfigurations(grpc::ServerContext* context, const UpdateConfig* request, ConfigAck* response) {
	std::string file = request->file();
	nlohmann::json jsonContent = nlohmann::json::parse(file);
	std::ofstream outputFile("Configurations.json");
	outputFile << jsonContent;

	if (true) {
		response->set_success(true);
		LOG_INFO("JSON upadte has sended successfully.");
	}
	else {
		response->set_success(false);
		LOG_INFO("Error while sending JSON update !");
	}
	return grpc::Status::OK;
}

int ServerService::MotionDetector(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold) {

	if (currentFrame.empty() || previousFrame.empty()) {
		throw std::invalid_argument("Input frames are empty");
	}

	if (currentFrame.size() != previousFrame.size() || currentFrame.type() != previousFrame.type()) {
		throw std::invalid_argument("Input frames have different sizes or types");
	}

	cv::Mat difference;
	cv::absdiff(currentFrame, previousFrame, difference);

	cv::cvtColor(difference, difference, cv::COLOR_BGR2GRAY);

	cv::Mat thresholded;
	cv::threshold(difference, thresholded, threshold, 255, cv::THRESH_BINARY);

	int numMotionPixels = cv::countNonZero(thresholded);

	return numMotionPixels;
}