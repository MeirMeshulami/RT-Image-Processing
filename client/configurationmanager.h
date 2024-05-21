#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

#include "CoreAPI.h"

 

class ConfigurationManager {
public:
    ConfigurationManager();

    ConfigurationManager(const QString& filePath);

    int getThresholdValue();

    void setThresholdValue(int value);

    void sendConfigsUpdates(API* api);

private:
    QString filePath;
    QJsonObject settingsObject;
    
    void readSettingsFromFile();

    void writeSettingsToFile();
};
