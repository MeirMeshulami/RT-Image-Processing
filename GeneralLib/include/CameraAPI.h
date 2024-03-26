#pragma once
#include <opencv2/core/utils/logger.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <json.hpp>
#include "CaptureAndPreprocess.h"
#include "FrameProcessor.h"
#include "LogManager.h"
#include "JsonManager.h"

class ServerAPI {
private:
	CaptureAndPreprocess captureAndPreprocess;
	nlohmann::json configJson;

public:

	ServerAPI() {
		LOG_INFO("\n\n===============================================\nCamera start running...\n===============================================\n\n");
	}

	void StartCamera() {
		SetLoggerLevel();
		captureAndPreprocess.Run(300, 0.08);
	}
	/*
	void StopServer() {
		frameProcessor.StopRunServer();
	}

	void StartStopShowingFrames() {
		frameProcessor.GetService().ControlShowingFrames();
	}

	void StartStopProcessingFrames() {
		frameProcessor.GetService().ControlProcessingFrames();
	}

	ThreadSafeQueue<FrameWithDetails>& GetFrameShowQueue() {
		return frameProcessor.GetService().GetFrameShowQueue();
	}
	*/
	void SetLoggerLevel()
	{
		JsonManager::CheckIfJsonModified(configJson);
		// First the QT has to change the level to the json file and then you have to call this function which will update the level in the program
		LogManager::GetInstance().SetLogLevel(configJson["log_settings"]["log_level"]);
	}
};
