#pragma once
#include <opencv2/opencv.hpp>
#include <chrono>
#include <mutex>

class FrameWithDetails
{
private:
    mutable std::mutex mutex_; // Mutex to protect member variable access
    cv::Mat frame;
    int frameNum;
    std::chrono::system_clock::time_point time;
    std::vector<cv::Rect> boxes;

public:
    FrameWithDetails();

    FrameWithDetails(FrameWithDetails&& other) noexcept;

    // Move assignment operator
    FrameWithDetails& operator=(FrameWithDetails&& other) noexcept;

    FrameWithDetails(const cv::Mat frame, const int frameNum, const std::chrono::system_clock::time_point time);

    ~FrameWithDetails();

    cv::Mat GetFrame() const;

    int GetFrameNum() const;

    std::chrono::system_clock::time_point GetFrameTime() const;

    std::vector<cv::Rect> GetBoxes() const;

    void SetBoxes(const std::vector<cv::Rect> boxes);
};
