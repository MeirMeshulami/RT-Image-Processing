#include <opencv2/core/utils/logger.hpp>
#include <fstream>
#include <filesystem>
#include <json.hpp>

#include "CaptureAndPreprocess.h"
#include "JsonManager.h"
#include "FrameSender.h"

int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	LOG_INFO("\n\n===============================================\nProgram start running...\n===============================================\n\n");

	try {
		nlohmann::json configJson;
		JsonManager::CheckIfJsonModified(configJson);

		LogManager::GetInstance().SetLogLevel(configJson["log_settings"]["log_level"]);

		CaptureAndPreprocess camera;
		std::atomic<bool> exitFlag(false);

		
		std::thread cameraThread([&camera, &configJson]() {
			while (camera.IsCameraRunning()) {
				try {
					
					JsonManager::CheckIfJsonModified(configJson);
					
					FrameSender frameSender = camera.ConnectToServer();
					camera.SendMotionFrames(frameSender);
				}
				catch (const std::exception& e) {
					LOG_ERROR("An exception occurred: {}", e.what());
					LOG_INFO("Connecting...");
				}
			}
			});

		std::thread keyboardThread([&camera, &exitFlag]() {
			while (!exitFlag) {
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					// if(cv::waitKey(100) == 27)
					camera.StopRunCamera();
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
	getchar();
	return 0;
}