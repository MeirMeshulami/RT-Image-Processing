#include "ServerService.h"
#include <opencv2/core/utils/logger.hpp>


int main() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

	LOG_INFO("\n\n===============================================\nCamera start running...\n===============================================\n\n");

	try {
		ServerService service;
		LogManager::GetInstance().SetLogLevel(service.configs["log_settings"]["log_level"]);

		service.RunServer();
	}
	catch (const std::exception& e) {
		LOG_ERROR("An exception occurred: {}", e.what());
	}

	LOG_INFO("Camera shut down.");
	return 0;
}