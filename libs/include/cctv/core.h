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

	API() {
		LOG_INFO("\n\n===============================================\nApplication start running...\n===============================================\n\n");
	}

	void Connect(std::string serverAddress) { client.Connect(serverAddress); }

	bool TryToConnect() { return client.TryToConnect(); }

	void Disconnect() { client.DestroyConnection(); }

	void StartStream() {
		frameProcessor.InitVideoWriter();
		client.StartStreamFrames();
	}

	void StopStream() {
		client.StopStreamFrames();
		frameProcessor.ReleaseVideoWriter();
	}

	//void EnableGPU(bool mode) { yolo.enableGpu = true; }

	cv::Mat Detect(cv::Mat& frame) { return yolo.Detect(frame); }

	void DisplayFPS(cv::Mat& img, long long start) { frameProcessor.DisplayFps(img, start); }

	std::atomic<bool> IsConnect() { return client.isConnect.load(); }

	std::atomic<bool>& IsDrawLabel() { return yolo.isDrawLabel; }

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

