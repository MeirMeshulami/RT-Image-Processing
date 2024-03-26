#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include <sqlite3.h>
#include <mutex>

class DBManager {
private:
    sqlite3* db;
    std::string dbPath;
    std::mutex dbMutex; // Mutex for protecting access to the database

    std::string FormatTimeToString(const std::chrono::system_clock::time_point& time);

public:
    void CreateDB(const std::string& path);

    ~DBManager();

    void CreateTable();

    void InsertData(const int frame_num, const cv::Rect& box, const cv::Scalar& avgRGB, const std::chrono::system_clock::time_point& time);
};
