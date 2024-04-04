#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include "FrameSender.h"
 

class ConfigurationManager {
public:
    ConfigurationManager(const QString& filePath);

    int getThresholdValue();

    void setThresholdValue(int value);

private:
    QString filePath;
    QJsonObject settingsObject;
    
    void readSettingsFromFile();

    void writeSettingsToFile();
};
