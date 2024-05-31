#pragma once
#include "ClientService.h"
//#include "DBManager.h"
#include "Frame.h"
#include "Settings.h"
#include "ThreadSafeQueue.h"
#include "YoloDetection.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <unordered_set>


class FrameProcessor {
public:
	cv::VideoWriter videoWriter;
	nlohmann::json configJson;
	//DBManager mDBManager;

	FrameProcessor();

	void InitVideoWriter();
	void ReleaseVideoWriter();
	void DisplayFps(cv::Mat& img, long long start);

	/*void InitTable();
	void CreateImageDirectory();
	void SaveFrameAsImage(const cv::Mat& frame, const int frameNum);
	void StartFrameProcessing();
	cv::Scalar CalcAverageRGB(const cv::Rect& box, const cv::Mat& frame);*/
};
