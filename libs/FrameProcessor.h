#pragma once
#include <filesystem>
#include <future>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <ctime>
#include "Frame.h"
#include "ThreadSafeQueue.h"
#include "YoloDetection.h"
#include "DBManager.h"
#include "JsonManager.h"
#include "ClientService.h"


class FrameProcessor {
public:
	std::shared_ptr<ThreadSafeQueue<Frame>> FrameProcessQueue;
	cv::VideoWriter mVideoWriter;
	DBManager mDBManager;
	std::mutex mDBMutex;
	std::vector<std::thread> frameProcessingThreads;
	std::thread frameReceiverThread;
	std::thread monitorKeyboardInputThread;
	std::atomic<bool> isConnect;
	std::shared_ptr<ClientService> service;
	std::shared_ptr<grpc::Channel> channel;
	std::filesystem::path videoPath;
	std::filesystem::path dbPath;
	std::filesystem::path imagePath;
	std::filesystem::path imagesDirectory;
	nlohmann::json configJson;
	std::unordered_set<std::string>classes;

	void InitializeVideoWriter();

	void InitializeTable();

	void CreateImageDirectory();

	void ReleaseVideoWriter();

	cv::Mat Detect(std::shared_ptr<Frame> image, YoloDetection& yolo);

	cv::Scalar CalculateAverageRGB(const cv::Rect& box, const cv::Mat& frame);

	void SaveFrameAsImage(const cv::Mat& frame, const int frameNum);

	void BuildConnection();

	FrameProcessor();

	void Connect();

	void StartFrameProcessing();

	void DestroyConnection();

	void StartStreamFrames();

	void StopStreamFrames();

	std::shared_ptr<ClientService> GetService();

	void enableGpuProcessing(cv::dnn::Net& net);

	void DisplayFps(cv::Mat& img, long long start);

};
