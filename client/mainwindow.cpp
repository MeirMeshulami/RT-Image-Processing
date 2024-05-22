#define _CRT_SECURE_NO_WARNINGS
#include "./ui_mainwindow.h"
#include "CheckComboBox.h"
#include "flowlayout.h"
#include "mainwindow.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QModelIndex>
#include <QProcess>



MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), isLive(false), isBrowsingLogFile(false)
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
	lowdLogFolderPath();
	CaptureImgfolderPath = "C:/MobileyeProjectTools/Output/images";
}

MainWindow::~MainWindow() { delete ui; }

///////========== Init Funcs ==========////////////
void MainWindow::init() {
	api.Connect();
	api.moveToThread(&frameDisplayThread);
	connect(&frameDisplayThread, &QThread::started, &api, &API::pollFramesForDisplay);
	std::ifstream file("Configurations.json");
	if (!file) {
		LOG_INFO("JSON file doesn't exist! ");
		return;
	}
	try {
		file >> configs;
		LOG_INFO("writing the JSON file into configs");
	}
	catch (nlohmann::json::parse_error& e) {
		LOG_INFO("Error parsing JSON: {}", e.what());
		return;
	}
	loadConfiguration();
	loadComboClasses();
	setWindowIcon(QIcon(R"(:logo.png)"));

	// display the Home page
	on_homeBtn_clicked();


}

void MainWindow::deInit() {
	on_stopLiveBtn_clicked();
	api.Disconnect();
}

void MainWindow::loadConfiguration() {

	int threshold = configs["camera_settings"]["threshold"];
	LOG_INFO("initial threshold is {}", threshold);
	ui->MotionValue->setText(QString::number(threshold));
	ui->threasholdSlider->setValue(threshold);
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
	on_refreshBtn_clicked();
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

///////========== Capture-Page Buttons ==========////////////
void MainWindow::on_refreshBtn_clicked()
{
	on_cleareBtn_clicked();
	display_capture_imgs(CaptureImgfolderPath);

}

void MainWindow::on_browseCapBtn_clicked()
{
	QString initialFolderPath = "C:/MobileyeProjectTools/Output/images";

	QFileDialog dialog(this, tr("Select Folder"), initialFolderPath);
	dialog.setFileMode(QFileDialog::Directory);  // Allow selecting directories
	dialog.setOption(QFileDialog::ShowDirsOnly, false);  // Show files within directories

	// Set name filters to display only image files
	QStringList nameFilters;
	nameFilters << "Images (*.png *.jpg *.jpeg *.bmp *.gif)";
	dialog.setNameFilters(nameFilters);

	// Execute the dialog and get the selected folder
	if (dialog.exec() == QDialog::Accepted) {
		QStringList selectedFiles = dialog.selectedFiles();
		if (!selectedFiles.isEmpty()) {
			// selectedFiles[0] will be the selected directory
			CaptureImgfolderPath = selectedFiles[0];
			on_cleareBtn_clicked();
			display_capture_imgs(CaptureImgfolderPath);
		}
	}
}

void MainWindow::display_capture_imgs(const QString& folderPath) {
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
	logFolderPath = QString::fromStdString(api.GetLogesDirectoryPath());
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
	ui->camFrame->setPixmap(scaledPixmap);

	QApplication::processEvents();
}

void MainWindow::on_liveBtn_clicked()
{
	if (!api.IsConnect()) {
		QMessageBox::warning(this, "Connection Error", "Camera is not connected!");
		return;
	}
	if (!isLive) {
		connect(&api, &API::frameReady, this, &MainWindow::displayFrame);
		// Gets frames from camera and push into a queue
		api.StartStream();
		// Pop frames from queue and emit displayFrame()
		frameDisplayThread.start();

		isLive = true;
	}
}

void MainWindow::on_stopLiveBtn_clicked()
{
	if (isLive) {
		disconnect(&api, &API::frameReady, this, &MainWindow::displayFrame);
		isLive = false;
		ui->camFrame->setPixmap(QPixmap());
		ui->camFrame->setText("Camera Offline");
		api.StopStream();
	}
}

void MainWindow::loadComboClasses() {
	QFile file("../../resources/CocoList.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qErrnoWarning("Can't load the class list!");
		std::cout << "Can't load the class list!" << std::endl;
		return;
	}

	CheckComboBox* comboBox = new CheckComboBox(this);
	comboBox->setPlaceholderText("Choose objects..");
	comboBox->view()->viewport()->installEventFilter(this);
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList classes = line.split(",", Qt::SkipEmptyParts);
		for (const QString& className : classes) {
			comboBox->addItem(className);
		}
	}
	file.close();

	QFrame* comboFrame = new QFrame(this);
	QVBoxLayout* frameLayout = new QVBoxLayout(comboFrame);
	frameLayout->setAlignment(Qt::AlignTop);
	frameLayout->addWidget(comboBox);
	ui->dashboardTools->layout()->addWidget(comboFrame);

	connect(comboBox, &CheckComboBox::checkedItemsChanged, [this](std::string className, bool isChecked) {
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

void MainWindow::on_threasholdSlider_valueChanged(int value)
{
	ui->MotionValue->setText(QString::number(value));
	configs["camera_settings"]["threshold"] = value;
	saveJson();
	//configs.sendConfigsUpdates(api);
}

void MainWindow::on_detectCheck_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked) {
		api.detection = true;
	}
	else {
		api.detection = false;
	}
}

void MainWindow::on_fpsCheck_stateChanged(int arg1)
{
	if (arg1 == Qt::Checked) {
		api.displayFps = true;
	}
	else {
		api.displayFps = false;
	}
}

void MainWindow::saveJson() {
	std::ofstream configFile("Configurations.json");

	// Write the JSON data to the file
	configFile << std::setw(4) << configs;

	// Close the file
	configFile.close();
}