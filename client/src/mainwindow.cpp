#define _CRT_SECURE_NO_WARNINGS
#include "./ui_mainwindow.h"
#include "logreader.h"
#include "mainwindow.h"

#include <iostream>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QModelIndex>
#include <QProcess>
#include <string>
#include <windows.h>
#include <VideoPlayer.h>
#include <qaudiooutput.h>



MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow),
isLive(false), isBrowsingLogFile(false), detection(false), displayFps(false), record(false)
{
#ifdef Q_OS_WIN
	// For Windows
	setWindowFlags(Qt::FramelessWindowHint);
#elif defined(Q_OS_MAC)
	// For macOS
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#endif
	ui->setupUi(this);

	init();
	show();

	QString logDirectoryPath = QString::fromStdString(configs["log_settings"]["log_directory"]);

	// Create and set up the log file reader
	LogReader* logReader = new LogReader(logDirectoryPath);
	QThread* logThread = new QThread();
	logReader->moveToThread(logThread);

	QObject::connect(logThread, &QThread::started, logReader, &LogReader::startReading);
	QObject::connect(logReader, &LogReader::newLogMessage, ui->textBrowser, &QTextBrowser::append);
	QObject::connect(logThread, &QThread::finished, logReader, &QObject::deleteLater);
	QObject::connect(logThread, &QThread::finished, logThread, &QObject::deleteLater);
	// Start the log file reading thread
	logThread->start();
}

MainWindow::~MainWindow() { delete ui; }

///////========== Init Funcs ==========////////////
void MainWindow::init() {
	Settings::ReadSettings(configs);
	api.Connect(getServerAddress());
	setConnectionStatus(api.IsConnect()); //TODO

	loadConfigs();
	loadComboBoxClasses();
	loadLogFolderPath();
	setWindowIcon(QIcon(R"(:logo.png)"));

	// display the Home page
	on_homeBtn_clicked();
}

void MainWindow::deInit() {
	on_stopLiveBtn_clicked();
	api.Disconnect();
}

void MainWindow::loadConfigs() {
    ////===== Camera Settings =====/////
	ui->applyBtn->hide();
	int threshold = configs["camera_settings"]["threshold"];
	ui->MotionValue->setText(QString::number(threshold));
	ui->threasholdSlider->setValue(threshold);

    int maxDiff=configs["camera_settings"]["max_diff_pixels"];
    ui->maxDiffValue->setText(QString::number(maxDiff));
    ui->maxDiffSlider->setValue(maxDiff);

    ////===== Network Settings =====/////
    std::string serverIP=configs["grpc_settings"]["camera_ip_address"];
    ui->serverIPvalue->setText(QString::fromStdString(serverIP));

    std::string port=configs["grpc_settings"]["port_number"];
    ui->portNumberBox->setValue(std::stoi(port));

    ////===== Yolo Settings =====/////
    std::string netModel= configs["yolo_settings"]["yolo_model"];
    ui->netModelList->setCurrentText(QString::fromStdString(netModel));

    ui->frameHeight->setValue(configs["yolo_settings"]["input_height"]);
    ui->frameWidth->setValue(configs["yolo_settings"]["input_width"]);
    ui->scoreThresholdVal->setValue(configs["yolo_settings"]["score_threshold"]);
    ui->NMSthresholdVal->setValue(configs["yolo_settings"]["nms_threshold"]);
    ui->confidanceThresholdVal->setValue(configs["yolo_settings"]["confidence_threshold"]);

    ////===== Log Settings =====/////
    std::string loggerLevel= configs["log_settings"]["log_level"];
    ui->logLevel->setCurrentText(QString::fromStdString(loggerLevel));

    std::string logFolder=configs["log_settings"]["log_directory"];
    ui->outputFolderPath->setText(QString::fromStdString(logFolder));
}

void MainWindow::displayLogs() {
	while (api.IsConnect()) {
		QCoreApplication::processEvents();
	}
}
///////========== Draging window Funcs ==========////////////
void MainWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragging = true;
		m_dragStartPosition = event->pos();
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		QPoint delta = event->pos() - m_dragStartPosition;
		move(pos() + delta);
		m_dragLastPosition = event->globalPos();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragging = false;
	}
}

///////========== Window Buttons ==========////////////
void MainWindow::on_extend_clicked()
{
	if (isFullScreen()) {
		showNormal();
		ui->extend->setIcon(QIcon(R"(:/feather/square.svg)"));
	}
	else {
		showFullScreen();
		ui->extend->setIcon(QIcon(R"(:/feather/copy.svg)"));
	}
}

void MainWindow::on_minimize_clicked()
{
	showMinimized();
}

void MainWindow::on_exit_clicked()
{
	MainWindow::deInit();
	QCoreApplication::exit();
}

///////========== Left side Menu Buttons ==========////////////
void MainWindow::on_homeBtn_clicked()
{
	ui->homeBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");
	ui->reportBtn->setStyleSheet("");
	ui->dataAnalBtn->setStyleSheet("");

	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_dataAnalBtn_clicked()
{
	ui->dataAnalBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");
	ui->reportBtn->setStyleSheet("");
	ui->homeBtn->setStyleSheet("");

	ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_reportBtn_clicked()
{
	ui->reportBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");
	ui->dataAnalBtn->setStyleSheet("");
	ui->homeBtn->setStyleSheet("");

	ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_SettingsBtn_clicked()
{
	ui->reportBtn->setStyleSheet("");
	ui->dataAnalBtn->setStyleSheet("");
	ui->homeBtn->setStyleSheet("");
	on_accountBtn_clicked();

	ui->stackedWidget->setCurrentIndex(3);
	on_appBtn_clicked();
}

void MainWindow::on_InformationBtn_clicked()
{
	ui->reportBtn->setStyleSheet("");
	ui->dataAnalBtn->setStyleSheet("");
	ui->homeBtn->setStyleSheet("");

	ui->stackedWidget->setCurrentIndex(4);
}

///////========== Logs-Page Buttons ==========////////////
QString MainWindow::findLatestLogFile(const QString& folderPath)
{
	QDir dir(folderPath);
	QStringList filters;
	filters << "log_*.txt"; // Assuming log files have names like "log_2023-08-29.txt"
	dir.setNameFilters(filters);
	QStringList fileList = dir.entryList(QDir::Files, QDir::Name | QDir::Reversed);
	if (!fileList.isEmpty())
	{
		return folderPath + fileList.first(); // The first file is the latest
	}
	return QString(); // No log files found
}

void MainWindow::updateLogTextBrowser()
{
	if (isBrowsingLogFile) {
		return; // Don't update the log content while browsing
	}

	loadLogFolderPath();
	QString latestLogFile = findLatestLogFile(logFolderPath);
	if (!latestLogFile.isEmpty())
	{
		currentLogFilePath = latestLogFile;
		QFile file(currentLogFilePath);
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream in(&file);
			QString logContent = in.readAll();
			file.close();

			if (logContent.isEmpty())
			{
				// If the log content is empty, display a message in the middle of the screen
				ui->textBrowser->setText("Empty Log File: " + latestLogFile);
				return; // Exit the function
			}

			ui->textBrowser->setText(logContent);
		}
	}
}

void MainWindow::loadLogFolderPath()
{
	logFolderPath = QString::fromStdString(configs["log_settings"]["log_directory"]);
	logFolderPath += QString::fromStdString("\\");
}

void MainWindow::on_showLogsBtn_clicked()
{

	updateLogTextBrowser();
}

void MainWindow::on_browseLogsBtn_clicked()
{
	isBrowsingLogFile = true;

	// Open a file dialog to select a log file
	QString filePath = QFileDialog::getOpenFileName(this, "Select a Log File", logFolderPath, "Log Files (*.txt)");

	if (!filePath.isEmpty()) {
		// Load and display the selected log file
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream in(&file);
			QString logContent = in.readAll();
			file.close();

			if (logContent.isEmpty())
			{
				// If the log content is empty, display a message
				ui->textBrowser->setText("Empty Log File: " + filePath);
			}
			else
			{
				ui->textBrowser->setText(logContent);
			}
		}
	}

	isBrowsingLogFile = false;
}

void MainWindow::refreshLogContent()
{
	updateLogTextBrowser();
}

///////========== Settings-Menu Buttons ==========////////////
void MainWindow::on_accountBtn_clicked()
{
	ui->accountBtn->setStyleSheet("background-color: white;color:black;");
	ui->SettingsMenu->setCurrentIndex(1);

	ui->feautersBtn->setStyleSheet("");
	ui->appBtn->setStyleSheet("");
}

void MainWindow::on_appBtn_clicked()
{
	ui->appBtn->setStyleSheet("background-color: white;color:black;");
	ui->SettingsMenu->setCurrentIndex(0);

	ui->feautersBtn->setStyleSheet("");
	ui->accountBtn->setStyleSheet("");
}

void MainWindow::on_feautersBtn_clicked()
{
	ui->feautersBtn->setStyleSheet("background-color: white;color:black;");
	ui->SettingsMenu->setCurrentIndex(2);

	ui->appBtn->setStyleSheet("");
	ui->accountBtn->setStyleSheet("");
}

///////========== Dashboard-Page Buttons ==========////////////
void MainWindow::displayFrame(const cv::Mat& image) {
	QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);

	QPixmap pixmap = QPixmap::fromImage(qImage);
	QPixmap scaledPixmap = pixmap.scaled(ui->camFrame->contentsRect().size(), Qt::KeepAspectRatio);

	if (isLive.load()) {
		ui->camFrame->setPixmap(scaledPixmap);
	}
}

void MainWindow::on_liveBtn_clicked()
{
	if (!api.IsConnect()) {
		connectionDialog();
	}
	bool wasLive = isLive.exchange(true);
	if (wasLive) {
		return;
	}

	// Gets frames from camera and push into a queue
	api.StartStream();
	// Pop frames from queue and emit displayFrame()
	pollFramesForDisplay();
}

void MainWindow::connectionDialog() {
	QMessageBox msgBox(this);
	msgBox.setWindowTitle("Connection Error");
	msgBox.setText("Camera is not connected!");
	msgBox.setIcon(QMessageBox::Warning);
	QPushButton* stayOfflineButton = msgBox.addButton("Stay Offline", QMessageBox::RejectRole);
	QPushButton* retryButton = msgBox.addButton("Retry", QMessageBox::AcceptRole);
	msgBox.exec();

	if (msgBox.clickedButton() == stayOfflineButton) {
		std::cout << "Stay Offline button clicked." << std::endl;
		return;
	}
	else if (msgBox.clickedButton() == retryButton) {
		std::cout << "Retry button clicked." << std::endl;
		bool sucess = api.RetryToConnect();
		if (!sucess) {
			QMessageBox::critical(this, "Connection Error", "Failed to connect to the server after several attempts !");
		}
	}
}

void MainWindow::on_stopLiveBtn_clicked()
{
	bool wasLive = isLive.exchange(false);
	if (!wasLive) {
		return;
	}
	ui->camFrame->clear();
	ui->camFrame->setText("Camera Offline");
	api.StopStream();
}

void MainWindow::pollFramesForDisplay() {

	std::thread pollFramesThread([this] {
		std::shared_ptr<Frame> frameToShow;
		while (isLive.load()) {
			if (api.GetFrameShowQueue()->TryPop(frameToShow)) {
				auto frame = frameToShow->GetFrame();
				auto start = cv::getTickCount();
				if (detection.load()) {
					frame = api.Detect(frame);
				}
				if (displayFps.load()) {
					api.DisplayFPS(frame, start);
				}
				if (record.load()) {
					api.GetVideoWriter().write(frame);
				}

				displayFrame(frame);
			}
		}
		});
	pollFramesThread.detach();
}

void MainWindow::loadComboBoxClasses() {
	ui->checkComboBox->setPlaceholderText("Choose Objects...");
	ui->checkComboBox->view()->viewport()->installEventFilter(this);


	connect(ui->checkComboBox, &CheckComboBox::checkedItemsChanged, [this](std::string className, bool isChecked) {
		this->modifyClassList(className, isChecked);
		});

}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::MouseButtonRelease) {
		return true;
	}

	return QObject::eventFilter(obj, event);
}

void MainWindow::modifyClassList(const std::string className, bool isChecked) {
	if (isChecked) {
		api.GetClassList().erase(className);
	}
	else {
		api.GetClassList().insert(className);
	}
}

///////========== Settings->Account  ==========////////////
void MainWindow::on_editName_clicked()
{
	if (ui->editName->text() == "Edit")
	{
		ui->nameText->setReadOnly(false);
		ui->nameText->setFocus();
		ui->editName->setText("Save");
		QIcon saveIcon(":/feather/save.svg");
		ui->editName->setIcon(saveIcon);
	}
	else if (ui->editName->text() == "Save")
	{
		ui->nameText->setReadOnly(true);
		QString newUsername = ui->nameText->text(); // Use text() instead of toPlainText() for QLineEdit
		ui->nameInBar->setText(newUsername);
		ui->editName->setText("Edit");
		QIcon editIcon(":/feather/edit.svg");
		ui->editName->setIcon(editIcon);
	}
}

void MainWindow::on_userBtn_clicked()
{
	// Open a file dialog to select an image
	QString filePath = QFileDialog::getOpenFileName(this, "Select an Image", "", "Images (*.png *.jpg *.bmp *.gif *.jpeg)");

	if (!filePath.isEmpty()) {
		// Load the selected image
		QPixmap pixmap(filePath);

		if (!pixmap.isNull()) {
			// Set the image as the button's icon and scale it to the button's size
			ui->userBtn->setIcon(QIcon(pixmap.scaled(ui->userBtn->size())));
			ui->userBtn->setIconSize(ui->userBtn->size());
		}
	}
}

///////========== Settings->Application  ==========////////////
void MainWindow::on_threasholdSlider_valueChanged(int value)
{
	ui->MotionValue->setText(QString::number(value));
	configs["camera_settings"]["threshold"] = value;
	ui->applyBtn->show();
}

void MainWindow::on_maxDiffSlider_valueChanged(int value)
{
    ui->maxDiffValue->setText(QString::number(value));
    configs["camera_settings"]["max_diff_pixels"] = value;
    ui->applyBtn->show();
}

void MainWindow::on_serverIPvalue_textEdited(const QString& arg1)
{
    configs["grpc_settings"]["camera_ip_address"] = ui->serverIPvalue->text().toStdString();
    ui->applyBtn->show();
}

void MainWindow::on_portNumberBox_valueChanged(int arg1)
{
    configs["grpc_settings"]["port_number"] = std::to_string(arg1);
    ui->applyBtn->show();
}

///////========== CheckBoxes Pannel  ==========////////////
void MainWindow::on_detectCheck_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked) {
		detection.store(true);
	}
	else {
		detection.store(false);
	}
}

void MainWindow::on_fpsCheck_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked) {
		displayFps.store(true);
	}
	else {
		displayFps.store(false);
	}
}

void MainWindow::on_drawCheck_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked) {
		api.IsDrawLabel().store(true);
	}
	else {
		api.IsDrawLabel().store(false);
	}
}

///////========== General Settings  ==========////////////
std::string MainWindow::getServerAddress() {
	std::string serverIP = configs["grpc_settings"]["camera_ip_address"];
	std::string port = configs["grpc_settings"]["port_number"];
	std::string serverAddress = serverIP + ":" + port;

	return serverAddress;
}

void MainWindow::on_applyBtn_clicked()
{
	ui->applyBtn->hide();
	Settings::SaveSettings(configs);
	api.UpdateServerSettings(configs);
}

void MainWindow::setConnectionStatus(bool isConnected) {

	/*std::thread isLiveThread([this, &isConnected] {
		while (true) {
			if (isConnected) {
				QPixmap icon(":/feather/green-online.png");
				ui->connectionIcon->setPixmap(icon);
				ui->connectionStatus->setText("Online");
			}
			else {
				QPixmap icon(":/feather/red-offline.png");
				ui->connectionIcon->setPixmap(icon);
				ui->connectionStatus->setText("Offline");
			}
		}
		});
	isLiveThread.detach();*/
}











