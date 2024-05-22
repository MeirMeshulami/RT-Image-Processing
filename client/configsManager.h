#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "CoreAPI.h"
#include "LogManager.h"



class ConfigurationManager {
public:
	ConfigurationManager();

	ConfigurationManager(const QString& filePath);

	int getThresholdValue();

	void setThresholdValue(int value);

	void sendConfigsUpdates(API& api);

private:
	QString filePath;
	QJsonObject settingsObject;
	nlohmann::json config;

	void readSettingsFromFile();

	void writeSettingsToFile();
};
