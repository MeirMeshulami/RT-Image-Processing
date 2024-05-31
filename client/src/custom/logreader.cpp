#include "LogReader.h"
#include <stdexcept>

LogReader::LogReader(const QString &filePath, QObject *parent)
    : QObject(parent), mFilePath(filePath), mFile(filePath)
{
    if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open log file");
    }
    mFile.seek(mFile.size());  // Start reading from the end of the file
}

void LogReader::startReading()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LogReader::readNewLines);
    timer->start(100);  // Check for new log messages every 100 ms
}

void LogReader::readNewLines()
{
    while (!mFile.atEnd()) {
        QString line = mFile.readLine();
        if (!line.isEmpty()) {
            emit newLogMessage(line.trimmed());
        }
    }
}
