#include "CaptureAndPreprocess.h"
#include "FrameSender.h"
#include <chrono>
#include <thread>

CaptureAndPreprocess::CaptureAndPreprocess() : mFrameCapture(cv::VideoCapture(0)), isCameraRunning(true){
    LOG_INFO("Capture frames started.");
    if (!mFrameCapture.isOpened()) {
        throw std::runtime_error("Failed to open camera.");
    }
    LOG_DEBUG("CaptureAndPreprocess was created successfully");
}

void CaptureAndPreprocess::Run(int frameCaptureDelay, float similarityThreshold) {
    JsonManager::CheckIfJsonModified(configJson);

    std::string ip = configJson["grpc_settings"]["camera_ip_address"];
    std::string port = configJson["grpc_settings"]["port_number"];
    std::string cameraAddress(ip + ":" + port);

    FrameSender frameSender(grpc::CreateChannel(cameraAddress, grpc::InsecureChannelCredentials()));
    LOG_INFO ("Created a channel to server.");

    int frameNum = 0;
    cv::Mat previousFrame;
    mFrameCapture.read(previousFrame);

    bool success = frameSender.SendFrame(previousFrame, frameNum++, std::chrono::system_clock::now());

    if (!success) {
        throw std::invalid_argument("Error: Failed to send the initial frame.");
    }

    while (isCameraRunning) {
        cv::Mat currentFrame;

        if (mFrameCapture.read(currentFrame)) {
            std::chrono::system_clock::time_point frameTime = std::chrono::system_clock::now();

            auto similarity = CalculateSimilarity(currentFrame, previousFrame);

            if (similarity > similarityThreshold) {
                success = frameSender.SendFrame(currentFrame, frameNum++, frameTime);
                if (!success) {
                    throw std::invalid_argument("Error: Failed to send a frame.");
                }
                previousFrame = currentFrame.clone();
            }
        }
    }
}

float CaptureAndPreprocess::CalculateSimilarity(const cv::Mat& currentFrame, const cv::Mat& previousFrame) {

    if (currentFrame.empty() || previousFrame.empty()) {
        throw std::invalid_argument("Input frames are empty");
    }

    if (currentFrame.size() != previousFrame.size() || currentFrame.type() != previousFrame.type()) {
        throw std::invalid_argument("Input frames have different sizes or types");
    }

    cv::Mat difference;
    cv::absdiff(currentFrame, previousFrame, difference);

    auto diffSum = static_cast<int>(cv::sum(difference)[0]);

    // Normalize the difference and return the similarity value
    // By dividing the sum of the absolute value by 255 * the number of pixels.
    // The return value will always be between 0.0 and 1.0.
    return static_cast<float>(static_cast<double>(diffSum) / (currentFrame.rows * currentFrame.cols * 255));
}

void CaptureAndPreprocess::StopRunCamera() {
    isCameraRunning = false;
}

std::atomic<bool>& CaptureAndPreprocess::GetIsCameraRunning() {
    return isCameraRunning;
}
