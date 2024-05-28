#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include "DBManager.h"
#include <stdexcept>

DBManager::~DBManager() {
	sqlite3_close(db);
}

void DBManager::CreateDB(const std::string& path) {
	dbPath = path;
	if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
		throw std::runtime_error("Unable to open/create database.");
}

void DBManager::CreateTable() {
	std::lock_guard<std::mutex> lock(dbMutex);

	std::string fieldDeclarations = "CREATE TABLE IF NOT EXISTS DetectionDetails("
		"Frame_num INT, "
		"Time TEXT, "
		"Top INT NOT NULL, "
		"Left INT NOT NULL, "
		"Width INT NOT NULL, "
		"Height INT NOT NULL, "
		"AvgR REAL NOT NULL, "
		"AvgG REAL NOT NULL, "
		"AvgB REAL NOT NULL"
		");";
	if (sqlite3_exec(db, fieldDeclarations.c_str(), NULL, 0, 0) != SQLITE_OK)
		throw std::runtime_error("Unable to create table.");
}
void DBManager::InsertData(const int frame_num, const cv::Rect& box, const cv::Scalar& avgRGB, const std::chrono::system_clock::time_point& time) {
	std::lock_guard<std::mutex> lock(dbMutex);

	auto timeString = FormatTimeToString(time);
	auto data = "INSERT INTO DetectionDetails (Frame_num, Time, Top, Left, Width, Height, AvgR, AvgG, AvgB) VALUES (" + std::to_string(frame_num) + ", '" + timeString + "', " + std::to_string(box.x) + ", " + std::to_string(box.y) + ", " + std::to_string(box.width) + ", " + std::to_string(box.height) + ", " + std::to_string(avgRGB[0]) + ", " + std::to_string(avgRGB[1]) + ", " + std::to_string(avgRGB[2]) + ");";
	if (sqlite3_exec(db, data.c_str(), NULL, 0, 0) != SQLITE_OK)
		throw std::runtime_error("Unable to insert data.");
}

std::string DBManager::FormatTimeToString(const std::chrono::system_clock::time_point& time) {
	auto timeT = std::chrono::system_clock::to_time_t(time);
	std::tm* timeInfo = std::localtime(&timeT);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;

	if (timeInfo == nullptr)
		throw std::runtime_error("Error converting time using localtime_s");

	char buffer[100];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S.", timeInfo);
	return buffer + std::to_string(ms.count());
}
