#pragma once
#include <opencv2/opencv.hpp>
#include "LogManager.h"
#include "JsonManager.h"
#include "FrameSender.h"


class CaptureAndPreprocess {
private:
    cv::VideoCapture mFrameCapture;
    std::atomic<bool> isCameraRunning;
    nlohmann::json configJson;

public:

    CaptureAndPreprocess();

    FrameSender CaptureAndPreprocess::ConnectToServer();

    int CalculateSimilarity(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold);

    void SendMotionFrames(FrameSender& frameSender);

    void StopRunCamera();

    std::atomic<bool>& IsCameraRunning();
};
