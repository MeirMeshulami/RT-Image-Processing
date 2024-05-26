#include "configsManager.h"
ConfigurationManager::ConfigurationManager() {
	std::ifstream file("Configurations.json");
	if (!file) {
		qWarning("JSON file doesn't exist! ");
		return;
	}

	try {
		file >> config;
	}
	catch (nlohmann::json::parse_error& e) {
		qWarning("Error parsing JSON: {}", e.what());
		return;
	}
}

ConfigurationManager::ConfigurationManager(const QString& filePath) :filePath("Configurations.json") { readSettingsFromFile(); }

void ConfigurationManager::readSettingsFromFile() {
	QFile configFile(filePath);
	if (!configFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open configuration file.";
		return;
	}

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll(), &error);
	configFile.close();

	if (doc.isNull()) {
		qWarning() << "Failed to parse configuration file:" << error.errorString();
		return;
	}

	settingsObject = doc.object();
}

void ConfigurationManager::writeSettingsToFile() {
	QFile configFile(filePath);
	if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		qWarning() << "Failed to open configuration file for writing.";
		return;
	}

	QJsonDocument doc(settingsObject);
	configFile.write(doc.toJson());
	configFile.close();
}

void ConfigurationManager::sendConfigsUpdates(API& api) {


	//file.close();
	std::string jsonStr = config.dump();
	if (api.IsConnect()) {
		auto stub = api.GetStub();
		bool success = stub->UpdateConfigs(jsonStr);

		if (!success)
			qWarning("Error while sending JSON update !");
		else
			qDebug("JSON upadte has sended successfully.");
	}
}
