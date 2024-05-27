#pragma once
#include "ClientService.h"
#include "FrameProcessor.h"
#include "JsonManager.h"
#include "LogManager.h"
#include <opencv2/core/utils/logger.hpp>
#include <QObject>

class API : public QObject {
	Q_OBJECT
private:
	FrameProcessor frameProcessor;
	ClientService client;
public:
	bool detection;
	bool displayFps;

	explicit API() :detection(false), displayFps(false) {
		LOG_INFO("\n\n===============================================\nApplication start running...\n===============================================\n\n");
	}

	bool IsConnect() { return client.isConnect; }

	void Connect(std::string serverAddress) {
		client.Connect(serverAddress);
	}

	void Disconnect() {
		client.DestroyConnection();
	}

	void StartStream() { client.StartStreamFrames(); }

	void StopStream() { client.StopStreamFrames(); }

	//void StartFrameProcessing() { frameProcessor.StartFrameProcessing(); }

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
public slots:

	void pollFramesForDisplay() {
		YoloDetection yolo;
		std::shared_ptr<Frame> frameToShow;

		while (IsConnect()) {
			if (GetFrameShowQueue()->TryPop(frameToShow)) {
				auto img = frameToShow->GetFrame();
				auto start = cv::getTickCount();
				if (detection) {
					img = frameProcessor.Detect(frameToShow, yolo);
				}
				if (displayFps) {
					frameProcessor.DisplayFps(img, start);
				}
				//LOG_INFO("displaying frame No. {} ", frameToShow->GetFrameNum());

				emit frameReady(img);
			}
		}
	}

signals:
	void frameReady(const cv::Mat& frame);

};
