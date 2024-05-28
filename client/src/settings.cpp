#include "settings.h"




void Settings::saveSettings(nlohmann::json& configs) {
	std::ofstream configFile("Configurations.json");
	configFile << std::setw(4) << configs;
	configFile.close();
}