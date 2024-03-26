#pragma once
#include <opencv2/opencv.hpp>
#include "LogManager.h"
#include "JsonManager.h"

class CaptureAndPreprocess {
private:
    cv::VideoCapture mFrameCapture;
    std::atomic<bool> isCameraRunning;
    nlohmann::json configJson;

public:

    CaptureAndPreprocess();

    float CalculateSimilarity(const cv::Mat& currentFrame, const cv::Mat& previousFrame);

    void Run(int frameCaptureDelay, float similarityThreshold = 0.05);

    void StopRunCamera();

    std::atomic<bool>& GetIsCameraRunning();
};
