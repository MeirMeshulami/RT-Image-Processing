#pragma once
#include "ClientService.h"
#include "FrameProcessor.h"
#include "LogManager.h"

class API {
private:
	FrameProcessor frameProcessor;
	ClientService client;
	Yolo yolo;
public:

	explicit API() {
		LOG_INFO("\n\n===============================================\nApplication start running...\n===============================================\n\n");
	}

	std::atomic<bool> IsConnect() { return client.isConnect.load(); }

	std::atomic<bool>& IsDrawLabel() { return yolo.isDrawLabel; }

	void Connect(std::string serverAddress) {
		client.Connect(serverAddress);
	}

	bool RetryToConnect() {
		return client.RetryToConnect();
	}

	void Disconnect() {
		client.DestroyConnection();
	}

	cv::Mat Detect(cv::Mat& frame) { return yolo.Detect(frame); }

	void DisplayFPS(cv::Mat& img, long long start) {
		frameProcessor.DisplayFps(img, start);
	}

	void StartStream() {
		frameProcessor.InitVideoWriter();
		client.StartStreamFrames();
	}

	void StopStream() {
		client.StopStreamFrames();
		frameProcessor.ReleaseVideoWriter();
	}

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue() {
		return client.GetFrameShowQueue();
	}

	std::unordered_set<std::string>& GetClassList() { return yolo.classes; };

	bool UpdateServerSettings(nlohmann::json& configs) {
		return client.UpdateServerSettings(configs);
	}

	cv::VideoWriter& GetVideoWriter() { return frameProcessor.videoWriter; }

	std::shared_ptr<spdlog::logger> GetLogger() {
		return LogManager::GetInstance().GetCommonLogger();
	}
};

