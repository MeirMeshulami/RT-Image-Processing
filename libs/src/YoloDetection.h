#pragma once
#include "LogManager.h"
#include "Settings.h"
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_set>

const auto BLACK = cv::Scalar(0, 0, 0);
const auto BLUE = cv::Scalar(255, 178, 50);
const auto YELLOW = cv::Scalar(0, 255, 255);
const auto RED = cv::Scalar(0, 0, 255);

// Text parameters.
const float FONT_SCALE = (float)0.7;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;


class Yolo
{
private:
	double INPUT_WIDTH;
	double INPUT_HEIGHT;
	double SCORE_THRESHOLD;
	double NMS_THRESHOLD;
	double CONFIDENCE_THRESHOLD;

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	nlohmann::json configJson;
	std::vector<std::string> classList;
	cv::dnn::Net net;

	void DrawLabel(cv::Mat& input_image, std::string label, int left, int top);

	void EnableGpuProcessing(cv::dnn::Net& net);

	std::vector<cv::Mat> PreProcess(const cv::Mat& inputImage);

	cv::Mat PostProcess(cv::Mat& input_image, std::vector<cv::Mat>& outputs, std::unordered_set<std::string>& classes);

	void LoadClassList();

	void LoadNet();

	void LoadSensitivities();

public:
	std::unordered_set<std::string>classes;
	std::atomic<bool>isDrawLabel;

	Yolo();

	cv::Mat Detect(cv::Mat& inputImage);



};

