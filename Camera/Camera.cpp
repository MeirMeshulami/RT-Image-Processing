#include <opencv2/core/utils/logger.hpp>
#include <fstream>
#include <filesystem>
#include <json.hpp>
#include "CaptureAndPreprocess.h"
#include "JsonManager.h"

int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	LOG_INFO("\n\n===============================================\nProgram start running...\n===============================================\n\n");

	try {
		nlohmann::json configJson;
		JsonManager::CheckIfJsonModified(configJson);

		LogManager::GetInstance().SetLogLevel(configJson["log_settings"]["log_level"]);

		CaptureAndPreprocess captureAndPreprocess;
		std::atomic<bool> exitFlag(false);

		std::thread cameraThread([&captureAndPreprocess, &configJson]() {
			while (captureAndPreprocess.GetIsCameraRunning()) {
				try {
					JsonManager::CheckIfJsonModified(configJson);
					captureAndPreprocess.Run(configJson["camera_settings"]["frame_capture_delay"], configJson["camera_settings"]["similarity_threshold"]);
				}
				catch (const std::exception& e) {
					LOG_ERROR("An exception occurred: {}", e.what());
					LOG_INFO("Connecting...");
				}
			}
			});

		std::thread keyboardThread([&captureAndPreprocess, &exitFlag]() {
			while (!exitFlag) {
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					captureAndPreprocess.StopRunCamera();
					exitFlag = true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to avoid high CPU usage
			}
			});
		cameraThread.join();
		keyboardThread.join();
	}
	catch (const std::exception& e) {
		LOG_ERROR("An exception occurred: {}", e.what());
	}

	LOG_INFO("Camera program shutting down.");

	return 0;
}