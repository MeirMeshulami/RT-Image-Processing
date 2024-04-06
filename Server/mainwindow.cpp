#include "mainwindow.h"
#include "flowlayout.h"
#include "./ui_mainwindow.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), isLive(false)
{

#ifdef Q_OS_WIN
	// For Windows
	setWindowFlags(Qt::FramelessWindowHint);
#elif defined(Q_OS_MAC)
	// For macOS
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#endif

	ui->setupUi(this);
	// Create an instance of ServerAPIController
	serverAPIController = new ServerAPIController(&serverAPI);

	// Create a thread for frame display
	QThread* frameDisplayThread = new QThread();

	// Move the serverAPIController to the frameDisplayThread
	serverAPIController->moveToThread(frameDisplayThread);

	// Connect the thread's started signal to the controller's frame display slot
	QObject::connect(frameDisplayThread, &QThread::started, serverAPIController, &ServerAPIController::pollFramesForDisplay);

	// Start the frame display thread
	frameDisplayThread->start();

	// Show the main window
	show();
	tear_up();

	lowdLogFolderPath();

}

MainWindow::~MainWindow()
{
	delete ui;
}

///////========== Init Funcs ==========////////////
void MainWindow::tear_up() {


	if (!serverAPIController->isServerRunning()) {
		serverAPIController->startServer();
	}
	loadConfiguration();
	setWindowIcon(QIcon(R"(:logo.png)"));

	// display the Home page
	ui->homeBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::tear_down() {
	if (serverAPIController->isServerRunning()) {
		serverAPIController->stopServer();
	}
	on_stopLiveBtn_clicked();
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
		m_dragLastPosition = event->globalPos();  // Store the last position
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
		showNormal(); // Restore to normal size
		ui->extend->setIcon(QIcon(R"(:/feather/square.svg)")); // Replace with your normal icon path
	}
	else {
		showFullScreen(); // Switch to full-screen mode
		ui->extend->setIcon(QIcon(R"(:/feather/copy.svg)")); // Replace with your full-screen icon path
	}
}

void MainWindow::on_minimize_clicked()
{
	showMinimized();
}

void MainWindow::on_exit_clicked()
{
	MainWindow::tear_down();
	QCoreApplication::exit();
}

///////========== Left side Menu Buttons ==========////////////
void MainWindow::on_homeBtn_clicked()
{
	ui->homeBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");

	// Reset styles for other buttons if necessary
	ui->reportBtn->setStyleSheet("");  // Reset stylesheet
	ui->dataAnalBtn->setStyleSheet("");  // Reset stylesheet

	// Switch to the HomePage in the QStackedWidget
	ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_dataAnalBtn_clicked()
{
	ui->dataAnalBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");

	// Reset styles for other buttons if necessary
	ui->reportBtn->setStyleSheet("");  // Reset stylesheet
	ui->homeBtn->setStyleSheet("");  // Reset stylesheet

	// Switch to the data analysis in the QStackedWidget
	ui->stackedWidget->setCurrentIndex(1);
	on_refreshBtn_clicked();
}

void MainWindow::on_reportBtn_clicked()
{
	ui->reportBtn->setStyleSheet("border-left: 3px solid orange; background-color:#1f2329;");

	// Reset styles for other buttons if necessary
	ui->dataAnalBtn->setStyleSheet("");  // Reset stylesheet
	ui->homeBtn->setStyleSheet("");  // Reset stylesheet

	// Switch to the data reports in the QStackedWidget
	ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_SettingsBtn_clicked()
{
	ui->reportBtn->setStyleSheet("");  // Reset stylesheet
	ui->dataAnalBtn->setStyleSheet("");  // Reset stylesheet
	ui->homeBtn->setStyleSheet("");  // Reset stylesheet
	on_accountBtn_clicked();
	// Switch to the Settings in the QStackedWidget
	ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_InformationBtn_clicked()
{
	ui->reportBtn->setStyleSheet("");  // Reset stylesheet
	ui->dataAnalBtn->setStyleSheet("");  // Reset stylesheet
	ui->homeBtn->setStyleSheet("");  // Reset stylesheet

	// Switch to the Settings in the QStackedWidget
	ui->stackedWidget->setCurrentIndex(4);
}

///////========== Capture-Page Buttons ==========////////////
void MainWindow::on_refreshBtn_clicked()
{
	on_cleareBtn_clicked();
	QString folderPath = "C:/MobileyeProjectTools/Output/images";
	QDir directory(folderPath);
	QStringList imageFilters;
	imageFilters << "*.png" << "*.jpg";
	QStringList imageFiles = directory.entryList(imageFilters, QDir::Files);

	// Define the desired image size (e.g., 200x200 pixels)
	int labelSizeWidth = 200;
	int labelSizeHeight = 200;

	FlowLayout* flowLayout = new FlowLayout;
	ui->flowGrid->setLayout(flowLayout);


	for (const QString& imageFile : imageFiles) {
		QLabel* imageLabel = new QLabel(this);
		imageLabel->setGeometry(10, 10, labelSizeWidth, labelSizeHeight);
		imageLabel->setScaledContents(false);

		QPixmap image(folderPath + "/" + imageFile);
		imageLabel->setPixmap(image.scaled(labelSizeWidth, labelSizeHeight, Qt::KeepAspectRatio));
		flowLayout->addWidget(imageLabel);
	}
}

void MainWindow::on_cleareBtn_clicked()
{
	qDeleteAll(ui->flowGrid->findChildren<FlowLayout*>());
	qDeleteAll(ui->flowGrid->findChildren<QLabel*>());
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

	lowdLogFolderPath();
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

void MainWindow::lowdLogFolderPath()
{
	logFolderPath = QString::fromStdString(serverAPI.GetLogesDirectoryPath());
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
	ui->SettingsMenu->setCurrentIndex(0);

	ui->feautersBtn->setStyleSheet("");
	ui->appBtn->setStyleSheet("");
}

void MainWindow::on_appBtn_clicked()
{
	ui->appBtn->setStyleSheet("background-color: white;color:black;");
	ui->SettingsMenu->setCurrentIndex(1);

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

///////========== Settings->Account Buttons ==========////////////
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

///////========== Dashboard-Page Buttons ==========////////////
void MainWindow::displayFrame(const cv::Mat& image) {
	QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);

	QPixmap pixmap = QPixmap::fromImage(qImage);
	QPixmap scaledPixmap = pixmap.scaled(ui->camFrame->contentsRect().size(), Qt::KeepAspectRatio);
	ui->camFrame->setPixmap(scaledPixmap);

	QApplication::processEvents();
}

void MainWindow::on_liveBtn_clicked()
{
	if (!isLive) {
		QObject::connect(serverAPIController, &ServerAPIController::frameReady, this, &MainWindow::displayFrame);
		isLive = true;
	}
}

void MainWindow::on_stopLiveBtn_clicked()
{
	if (isLive) {
		QObject::disconnect(serverAPIController, &ServerAPIController::frameReady, this, &MainWindow::displayFrame);
		isLive = false;
		ui->camFrame->setPixmap(QPixmap());
		ui->camFrame->setText("Camera Offline");

	}
}


void MainWindow::loadConfiguration() {
	configurationManager = new ConfigurationManager("Configurations.json");
	int threshold = configurationManager->getThresholdValue();
	ui->MotionValue->setText(QString::number(threshold));
	ui->threasholdSlider->setValue(threshold);
}

void MainWindow::on_threasholdSlider_valueChanged(int value)
{
	ui->MotionValue->setText(QString::number(value));
	configurationManager->setThresholdValue(value);

	CameraSettings settings;
	settings.set_threshold(value);

	grpc::ClientContext context;
	google::protobuf::Empty response;
	grpc::Status status = configurationManager->configsStub->UpdateCameraSettings(&context, settings, &response);

}

