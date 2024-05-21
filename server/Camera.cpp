#include <opencv2/core/utils/logger.hpp>
#include <fstream>
#include <filesystem>
#include <json.hpp>

#include "JsonManager.h"
#include "ServerService.h"


int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	LOG_INFO("\n\n===============================================\nCamera start running...\n===============================================\n\n");

	try {
		nlohmann::json configJson;
		JsonManager::CheckIfJsonModified(configJson);

		LogManager::GetInstance().SetLogLevel(configJson["log_settings"]["log_level"]);

		ServerService service;
		std::atomic<bool> exitFlag(false);

		
		std::thread cameraThread([&service, &configJson]() {
			while (service.IsCameraRunning()) {
				try {
					JsonManager::CheckIfJsonModified(configJson);
					service.RunServer();
				}
				catch (const std::exception& e) {
					LOG_ERROR("An exception occurred: {}", e.what());
					LOG_INFO("Connecting...");
				}
			}
			});

		std::thread keyboardThread([&service, &exitFlag]() {
			while (!exitFlag) {
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					// if(cv::waitKey(100) == 27)
					service.StopRunCamera();
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