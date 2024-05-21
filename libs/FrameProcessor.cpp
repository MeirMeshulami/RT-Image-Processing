#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include "FrameProcessor.h"


FrameProcessor::FrameProcessor() :isConnect(false) {
	JsonManager::CheckIfJsonModified(configJson);

#ifdef _WIN32
	std::string os = "_windows";
#elif __linux__
	std::string os = "_linux";
#endif
	std::string path = configJson["output_settings"]["video_path" + os];
	videoPath = path;
	path = configJson["output_settings"]["db_path" + os];
	dbPath = path;
	path = configJson["output_settings"]["image_path" + os];
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
	service = std::make_shared<ClientService>(channel);
	LOG_INFO("Client connected.");
	isConnect = true;
}

void FrameProcessor::StartStreamFrames() {
	std::thread streamThread([&] {
		GetService()->stopStreaming.store(false);
		GetService()->GetFrame();
		});
	streamThread.detach();
}

void FrameProcessor::StopStreamFrames() {
	GetService()->stopStreaming.store(true);
	isConnect.store(false);
}

void FrameProcessor::StartFrameProcessing() {
	InitializeTable();
	CreateImageDirectory();
}

cv::Mat FrameProcessor::Detect(std::shared_ptr<Frame> frame, YoloDetection&yolo) {
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
	auto fps = 1.0 / elapsedTime;
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << fps;
	std::string fpsText_after_processing = "FPS: " + stream.str();
	cv::putText(img, fpsText_after_processing, cv::Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
}

void FrameProcessor::DestroyConnection() {
	LOG_INFO("Disconnecting from camera...");
	isConnect = false;
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



