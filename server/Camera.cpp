#include "JsonManager.h"
#include "ServerService.h"
#include <opencv2/core/utils/logger.hpp>


int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	LOG_INFO("\n\n===============================================\nCamera start running...\n===============================================\n\n");

	try {
		ServerService service;
		JsonManager& json = service.configs;
		auto& configJson = json.configJson;
		json.CheckIfJsonModified();

		LogManager::GetInstance().SetLogLevel(configJson["log_settings"]["log_level"]);


		std::atomic<bool> exitFlag(false);

		std::thread cameraThread([&service, &json]() {
			try {
				service.RunServer();
			}
			catch (const std::exception& e) {
				LOG_ERROR("An exception occurred: {}", e.what());
				LOG_INFO("Connecting...");
			}
			});
		cameraThread.join();
	}
	catch (const std::exception& e) {
		LOG_ERROR("An exception occurred: {}", e.what());
	}

	LOG_INFO("Camera shut down.");
	return 0;
}