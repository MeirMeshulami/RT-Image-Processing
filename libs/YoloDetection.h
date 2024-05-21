#pragma once
#include <unordered_set>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
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
	const double INPUT_WIDTH = 640.0;
	const double INPUT_HEIGHT = 640.0;
	const double SCORE_THRESHOLD = 0.5;
	const double NMS_THRESHOLD = 0.45;
	const double CONFIDENCE_THRESHOLD = 0.45;
	std::mutex yoloMutex;
	
	
	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	nlohmann::json configJson;
	int inputWidth;
	int inputHeight;
	float confidenceThreshold;
	float scoreThreshold;
	float nmsThreshold;
	
	void draw_label(cv::Mat& input_image, std::string label, int left, int top);


public:
	cv::dnn::Net net;
	std::vector<std::string> classList;
	YoloDetection();

	std::vector<cv::Mat> PreProcess(const cv::Mat& inputImage);
	
	cv::Mat post_process(cv::Mat& input_image, std::vector<cv::Mat>& outputs, std::unordered_set<std::string>& classes);
};

