#ifndef LOGREADER_H
#define LOGREADER_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include <QTextStream>

class LogReader : public QObject
{
    Q_OBJECT

public:
    explicit LogReader(const QString &directoryPath, QObject *parent = nullptr);

signals:
    void newLogMessage(const QString &msg);

public slots:
    void startReading();

private slots:
    void readNewLines();

private:
    QString getLogFileName() const;
    void openLogFile();

    QString mDirectoryPath;
    QString mCurrentLogFilePath;
    QFile mFile;
};

#endif // LOGREADER_H
