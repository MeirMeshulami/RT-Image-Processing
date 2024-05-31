#include "LogReader.h"
#include <QDateTime>
#include <stdexcept>

LogReader::LogReader(const QString &directoryPath, QObject *parent)
    : QObject(parent), mDirectoryPath(directoryPath)
{
    mCurrentLogFilePath = getLogFileName();
    openLogFile();
}

QString LogReader::getLogFileName() const
{
    // Generate the log file name based on the current date
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    return mDirectoryPath + "/log_" + date + ".txt";
}

void LogReader::openLogFile()
{
    mFile.setFileName(mCurrentLogFilePath);
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
    // Check if the log file name has changed (new day)
    QString newLogFilePath = getLogFileName();
    if (newLogFilePath != mCurrentLogFilePath) {
        mCurrentLogFilePath = newLogFilePath;
        openLogFile();
    }

    while (!mFile.atEnd()) {
        QString line = mFile.readLine();
        if (!line.isEmpty()) {
            emit newLogMessage(line.trimmed());
        }
    }
}
