#define _CRT_SECURE_NO_WARNINGS
#include "FrameProcessor.h"
#include <ctime>


FrameProcessor::FrameProcessor() :isConnect(false) {
	JsonManager::ReadSettings(configJson);
	std::string path = configJson["output_settings"]["video_path"];
	videoPath = path;
	path = configJson["output_settings"]["db_path"];
	dbPath = path;
	path = configJson["output_settings"]["image_path"];
	imagePath = path;
	mDBManager.CreateDB(dbPath.string());

	InitializeVideoWriter();
}

void FrameProcessor::Connect() {
	LOG_DEBUG("Starting frame receiver thread...");
	frameReceiverThread = std::thread(&FrameProcessor::BuildConnection, this);
	frameReceiverThread.detach();
}

void FrameProcessor::BuildConnection() {
	LOG_INFO("Client connecting...");
	channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

	auto state = channel->GetState(true);
	int retries = 30;
	while (state != grpc_connectivity_state::GRPC_CHANNEL_READY && retries > 0) {
		LOG_INFO("Waiting for channel to be ready, current state: {}", state);

		channel->WaitForStateChange(state, std::chrono::system_clock::now() + std::chrono::seconds(10));
		state = channel->GetState(true);
		retries--;
	}

	if (state == grpc_connectivity_state::GRPC_CHANNEL_READY) {
		service = std::make_shared<ClientService>(channel);
		LOG_INFO("Client connected.");
		isConnect.store(true);
	}
	else {
		LOG_ERROR("Failed to connect to the server after several attempts.");
		isConnect.store(false);
	}
}

void FrameProcessor::StartStreamFrames() {
	std::thread streamThread([&] {
		GetService()->stopStreaming.store(false);
		GetService()->GetFrame();
		});
	streamThread.detach();
}

void FrameProcessor::StopStreamFrames() {
	LOG_INFO("Stopping stream..");
	GetService()->stopStreaming.store(true);
}

void FrameProcessor::StartFrameProcessing() {
	InitializeTable();
	CreateImageDirectory();
}

cv::Mat FrameProcessor::Detect(std::shared_ptr<Frame> frame, YoloDetection& yolo) {
	auto start = cv::getTickCount();
	auto detections = yolo.PreProcess(frame->GetFrame());
	cv::Mat img = (yolo.post_process(frame->GetFrame(), detections, classes));



	/*int frameNum = image->GetFrameNum();
	cv::Mat frame = image->GetFrame();*/
	/*if (image->GetBoxes().size() > 0) {
		mVideoWriter.write(frame);
		SaveFrameAsImage(frame, frameNum);
	}
	for (auto& box : image->GetBoxes()) {
		auto avgRGB = CalculateAverageRGB(box, frame);
		mDBManager.InsertData(frameNum, box, avgRGB, image->GetFrameTime());
	}*/

	return img;
}

void FrameProcessor::DisplayFps(cv::Mat& img, long long start) {
	auto end = cv::getTickCount();
	auto elapsedTime = (end - start) / cv::getTickFrequency();
	int fps = 1 / elapsedTime;
	std::stringstream stream;
	int maxFps = 999;
	fps = fps > maxFps ? maxFps : fps;
	std::string fpsText_after_processing = "FPS: " + std::to_string(fps);
	cv::putText(img, fpsText_after_processing, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
}

void FrameProcessor::DestroyConnection() {
	LOG_INFO("Disconnecting from camera...");
	isConnect.store(false);

	ReleaseVideoWriter();
	LOG_INFO("Disconnected.");
}

void FrameProcessor::ReleaseVideoWriter() {
	LOG_INFO("Releasing video writer...");
	mVideoWriter.release();
	LOG_INFO("Video writer released.");
}

cv::Scalar FrameProcessor::CalculateAverageRGB(const cv::Rect& box, const cv::Mat& frame) {
	cv::Mat subImg = frame(box);
	cv::Scalar avgColor = cv::mean(subImg);
	return avgColor;
}

void FrameProcessor::SaveFrameAsImage(const cv::Mat& frame, const int frameNum) {
	std::string imageName = "frame_" + std::to_string(frameNum) + ".png";
	cv::imwrite(imagesDirectory.string() + "/" + imageName, frame);
}

void FrameProcessor::InitializeVideoWriter() {
	LOG_DEBUG("Creating video writer...");
	mVideoWriter.open(videoPath.string(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 3, cv::Size(640, 480));

	if (!mVideoWriter.isOpened()) {
		throw std::runtime_error("Failed to open video writer.");
	}

	LOG_DEBUG("Video writer initialized.");
}

void FrameProcessor::InitializeTable() {
	LOG_INFO("Creating database table...");
	mDBManager.CreateTable();
	LOG_INFO("Database table created.");
}

void FrameProcessor::CreateImageDirectory() {
	auto now = std::chrono::system_clock::now();
	auto inTimeT = std::chrono::system_clock::to_time_t(now);
	std::tm currentTm = *std::localtime(&inTimeT);

	std::stringstream dateSs;
	dateSs << std::put_time(&currentTm, "%Y-%m-%d");
	std::filesystem::path dateFolder = imagePath / dateSs.str();

	std::stringstream timeSs;
	timeSs << std::put_time(&currentTm, "%H-%M-%S");
	std::filesystem::path timeSubfolder = dateFolder / timeSs.str();

	if (!std::filesystem::exists(dateFolder)) {
		std::filesystem::create_directory(dateFolder);
	}

	if (!std::filesystem::exists(timeSubfolder)) {
		std::filesystem::create_directory(timeSubfolder);
	}

	imagesDirectory = timeSubfolder;
}

std::shared_ptr<ClientService> FrameProcessor::GetService() { return service; }

void FrameProcessor::enableGpuProcessing(cv::dnn::Net& net) {
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}



