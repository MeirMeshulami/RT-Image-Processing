#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include "LogManager.h"
#include "JsonManager.h"

const auto BLACK = cv::Scalar(0, 0, 0);
const auto BLUE = cv::Scalar(255, 178, 50);
const auto YELLOW = cv::Scalar(0, 255, 255);
const auto RED = cv::Scalar(0, 0, 255);

// Text parameters.
const float FONT_SCALE = (float)0.7;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;


class YoloDetection
{
private:
	std::mutex yoloMutex;
	cv::dnn::Net net;
	std::vector<std::string> classList;
	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	nlohmann::json configJson;
	int inputWidth;
	int inputHeight;
	float confidenceThreshold;
	float scoreThreshold;
	float nmsThreshold;
	
	void DrawLabel(const cv::Mat& inputImage, const std::string label, const int left, int top);

	void DrawBoundingBox(const cv::Mat& frame, const cv::Rect& box, const float confidence, const int classId);

public:
	YoloDetection();

	std::vector<cv::Mat> PreProcess(const cv::Mat& inputImage);
	
	std::vector<cv::Rect> PostProcess(const cv::Mat &inputImage, const std::vector<cv::Mat>& outputs);
};

