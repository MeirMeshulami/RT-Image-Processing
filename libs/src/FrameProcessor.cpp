#define _CRT_SECURE_NO_WARNINGS
#include "FrameProcessor.h"
#include <ctime>


FrameProcessor::FrameProcessor() {
	Settings::ReadSettings(configJson);
}

void FrameProcessor::InitVideoWriter() {
	LOG_DEBUG("Creating video writer...");
	std::string videoPath = configJson["output_settings"]["video_path"];

	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&currentTime), "%d-%m-%Y_%H-%M-%S");
	std::string dateTimeStr = ss.str();

	videoPath += "record_" + dateTimeStr + ".mp4";
	std::string outDir = configJson["output_settings"]["out_dir_path"];
	std::filesystem::create_directory(outDir + "records");

	videoWriter.open(videoPath, cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 15, cv::Size(640, 480));

	if (!videoWriter.isOpened()) {
		throw std::runtime_error("Failed to open video writer.");
	}

	LOG_DEBUG("Video writer initialized with file: {}", videoPath);
}

void FrameProcessor::ReleaseVideoWriter() {
	LOG_INFO("Releasing video writer...");
	videoWriter.release();
	LOG_INFO("Video writer released.");
}

void FrameProcessor::DisplayFps(cv::Mat& img, long long start) {
	auto end = cv::getTickCount();
	auto elapsedTime = (end - start) / cv::getTickFrequency();
	unsigned int fps = 1 / elapsedTime;
	std::stringstream stream;
	int maxFps = 999;
	fps = fps > maxFps ? maxFps : fps;
	std::string fpsText_after_processing = "FPS: " + std::to_string(fps);
	cv::putText(img, fpsText_after_processing, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
}

//void FrameProcessor::CreateImageDirectory() {
//	auto now = std::chrono::system_clock::now();
//	auto inTimeT = std::chrono::system_clock::to_time_t(now);
//	std::tm currentTm = *std::localtime(&inTimeT);
//
//	std::stringstream dateSs;
//	dateSs << std::put_time(&currentTm, "%Y-%m-%d");
//	std::filesystem::path dateFolder = imagePath / dateSs.str();
//
//	std::stringstream timeSs;
//	timeSs << std::put_time(&currentTm, "%H-%M-%S");
//	std::filesystem::path timeSubfolder = dateFolder / timeSs.str();
//
//	if (!std::filesystem::exists(dateFolder)) {
//		std::filesystem::create_directory(dateFolder);
//	}
//
//	if (!std::filesystem::exists(timeSubfolder)) {
//		std::filesystem::create_directory(timeSubfolder);
//	}
//
//	imagesDirectory = timeSubfolder;
//}

//void FrameProcessor::InitTable() {
// // init DB needed
// //mDBManager.CreateDB(dbPath.string());
// 
//	LOG_INFO("Creating database table...");
//	mDBManager.CreateTable();
//	LOG_INFO("Database table created.");
//}

//void FrameProcessor::SaveFrameAsImage(const cv::Mat& frame, const int frameNum) {
//	std::string imageName = "frame_" + std::to_string(frameNum) + ".png";
//	cv::imwrite(imagesDirectory.string() + "/" + imageName, frame);
//}

//cv::Scalar FrameProcessor::CalcAverageRGB(const cv::Rect& box, const cv::Mat& frame) {
//	cv::Mat subImg = frame(box);
//	cv::Scalar avgColor = cv::mean(subImg);
//	return avgColor;
//}

//void FrameProcessor::StartFrameProcessing() {
//	InitTable();
//	CreateImageDirectory();
//}

//cv::Mat FrameProcessor::Detect(std::shared_ptr<Frame> frame) {
//	Yolo yolo;
//	auto start = cv::getTickCount();
//	auto detections = yolo.PreProcess(frame->GetFrame());
//	cv::Mat img = (yolo.PostProcess(frame->GetFrame(), detections, classes));
//
//
//
//	int frameNum = image->GetFrameNum();
//	cv::Mat frame = image->GetFrame();
//	if (image->GetBoxes().size() > 0) {
//		mVideoWriter.write(frame);
//		SaveFrameAsImage(frame, frameNum);
//	}
//	for (auto& box : image->GetBoxes()) {
//		auto avgRGB = CalculateAverageRGB(box, frame);
//		mDBManager.InsertData(frameNum, box, avgRGB, image->GetFrameTime());
//	}
//
//	return img;
//}



