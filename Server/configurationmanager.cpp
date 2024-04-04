#include "configurationmanager.h"

ConfigurationManager::ConfigurationManager(const QString& filePath) : filePath(filePath) {
    readSettingsFromFile();

}

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

int ConfigurationManager::getThresholdValue() {
    QJsonObject cameraSettings = settingsObject.value("camera_settings").toObject();
    return cameraSettings.value("threshold").toInt();
}

void ConfigurationManager::setThresholdValue(int value) {
    QJsonObject cameraSettings = settingsObject.value("camera_settings").toObject();
    cameraSettings["threshold"] = value;
    settingsObject["camera_settings"] = cameraSettings;

    writeSettingsToFile();
}
