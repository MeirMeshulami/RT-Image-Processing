#pragma once
#include "FrameProcessor.h"
#include "JsonManager.h"
#include "LogManager.h"
#include <opencv2/core/utils/logger.hpp>
#include <QObject>

class API : public QObject {
	Q_OBJECT
private:
	FrameProcessor frameProcessor;
public:
	bool detection;
	bool displayFps;

	explicit API() :detection(false), displayFps(false) {
		LOG_INFO("\n\n===============================================\nApplication start running...\n===============================================\n\n");
	}


	bool IsConnect() { return frameProcessor.isConnect; }

	void Connect() {
		frameProcessor.Connect();
	}

	void Disconnect() {
		frameProcessor.DestroyConnection();
	}

	void StartStream() { frameProcessor.StartStreamFrames(); }

	void StopStream() { frameProcessor.StopStreamFrames(); }

	void StartFrameProcessing() { frameProcessor.StartFrameProcessing(); }

	void StopShowingFrames() { frameProcessor.GetService()->ControlShowingFrames(); }

	void StopProcessingFrames() { frameProcessor.GetService()->ControlProcessingFrames(); }

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue() {
		return frameProcessor.GetService()->GetFrameShowQueue();
	}

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameProcessQueue() {
		return frameProcessor.GetService()->GetFrameProcessQueue();
	}

	std::unordered_set<std::string>& GetClassList() { return frameProcessor.classes; };

	void pushEmptyFrame(cv::Mat frame) {
		GetFrameProcessQueue()->Push(std::make_shared<Frame>(frame, -1, std::chrono::system_clock::now()));
	}

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
