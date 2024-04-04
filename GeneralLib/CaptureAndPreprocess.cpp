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

FrameSender CaptureAndPreprocess::ConnectToServer() {
	JsonManager::CheckIfJsonModified(configJson);

	std::string cameraIp = configJson["grpc_settings"]["camera_ip_address"];
	std::string cameraPort = configJson["grpc_settings"]["port_number"];
	std::string cameraAddress(cameraIp + ":" + cameraPort);
	

	std::shared_ptr<grpc::Channel> channel =grpc::CreateChannel(cameraAddress, grpc::InsecureChannelCredentials());
	if (!channel) {
		throw std::runtime_error("Failed to create channel to server.");
	}
	FrameSender frameSender(channel);
	LOG_INFO("Created a channel to server.");

	return frameSender;
}

void CaptureAndPreprocess::SendMotionFrames(FrameSender& frameSender) {
    JsonManager::CheckIfJsonModified(configJson);

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
			
			double threshold = configJson["camera_settings"]["threshold"];
			auto numMotionPixels = CalculateSimilarity(currentFrame, previousFrame, threshold);

			int maxDiffPixels = configJson["camera_settings"]["max_diff_pixels"];

			if (numMotionPixels > maxDiffPixels) {
				success = frameSender.SendFrame(currentFrame, frameNum++, frameTime);
				if (!success) {
					throw std::invalid_argument("Error: Failed to send a frame.");
				}
				previousFrame = currentFrame;
			}
		}
	}
}

int CaptureAndPreprocess::CalculateSimilarity(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold) {

	if (currentFrame.empty() || previousFrame.empty()) {
		throw std::invalid_argument("Input frames are empty");
	}

	if (currentFrame.size() != previousFrame.size() || currentFrame.type() != previousFrame.type()) {
		throw std::invalid_argument("Input frames have different sizes or types");
	}

	cv::Mat difference;
	cv::absdiff(currentFrame, previousFrame, difference);

	cv::cvtColor(difference, difference, cv::COLOR_BGR2GRAY);

	cv::Mat thresholded;
	cv::threshold(difference, thresholded, threshold, 255, cv::THRESH_BINARY);

	int numMotionPixels = cv::countNonZero(thresholded);

	return numMotionPixels;
}

void CaptureAndPreprocess::StopRunCamera() {
	isCameraRunning = false;
}

std::atomic<bool>& CaptureAndPreprocess::IsCameraRunning() {
	return isCameraRunning;
}
