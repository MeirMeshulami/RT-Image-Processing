#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>
#include "./ui_mainwindow.h"
#include "ServerAPIController.h"
#include "configurationmanager.h"

#include <QtGui>
#include <QStyle>
#include <QLayout>
#include <QRect>
#include <QWidgetItem>
#include "ServerAPI.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ServerAPI serverAPI;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void displayFrame(const cv::Mat& frame);
    QString findLatestLogFile(const QString& folderPath);
    void updateLogTextBrowser();
    void lowdLogFolderPath();
    void loadConfiguration();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_homeBtn_clicked();

    void on_dataAnalBtn_clicked();

    void on_reportBtn_clicked();

    void on_exit_clicked();

    void on_refreshBtn_clicked();

    void on_extend_clicked();

    void on_minimize_clicked();

    void on_cleareBtn_clicked();

    void on_liveBtn_clicked();

    void on_SettingsBtn_clicked();

    void on_showLogsBtn_clicked();

    void on_browseLogsBtn_clicked();

    void refreshLogContent();

    void on_InformationBtn_clicked();

    void tear_up();

    void tear_down();

    void on_accountBtn_clicked();

    void on_appBtn_clicked();

    void on_feautersBtn_clicked();

    void on_editName_clicked();

    void on_userBtn_clicked();

    void on_stopLiveBtn_clicked();

    void on_threasholdSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    bool m_dragging;
    QPoint m_dragStartPosition;
    QPoint m_dragLastPosition;
    bool isLive;
    ServerAPIController* serverAPIController;
    ConfigurationManager *configurationManager;
    QString logFolderPath;
    QString currentLogFilePath;
    bool isBrowsingLogFile = false;
};

#endif // MAINWINDOW_H
