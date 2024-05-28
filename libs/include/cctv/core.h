#pragma once
#include "ClientService.h"
#include "FrameProcessor.h"

class API {
private:
	FrameProcessor frameProcessor;
	ClientService client;
public:

	explicit API() {
		LOG_INFO("\n\n===============================================\nApplication start running...\n===============================================\n\n");
	}

	bool IsConnect() { return client.isConnect; }

	void Connect(std::string serverAddress) {
		client.Connect(serverAddress);
	}

	void Disconnect() {
		client.DestroyConnection();
	}

	cv::Mat Detect(std::shared_ptr<Frame>& frame) {
		return frameProcessor.Detect(frame);
	}

	void DisplayFPS(cv::Mat& img, long long start) {
		frameProcessor.DisplayFps(img, start);
	}

	void StartStream() { client.StartStreamFrames(); }

	void StopStream() { client.StopStreamFrames(); }

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue() {
		return client.GetFrameShowQueue();
	}

	std::unordered_set<std::string>& GetClassList() { return frameProcessor.classes; };

	std::shared_ptr<ClientService> GetStub() {
		auto service = frameProcessor.GetService();
		if (!service)
			LOG_ERROR("Service is null !");
		return service;
	}

};
