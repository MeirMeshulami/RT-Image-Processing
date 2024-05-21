#pragma once
#include <opencv2/opencv.hpp>
#include <chrono>
#include <mutex>

class Frame
{
private:
    mutable std::mutex mutex_; 
    cv::Mat frame;
    int frameNum;
    std::chrono::system_clock::time_point time;
    std::vector<cv::Rect> boxes;

public:
    Frame();

    Frame(Frame&& other) noexcept;

    Frame& operator=(Frame&& other) noexcept;

    Frame(const cv::Mat frame, const int frameNum, const std::chrono::system_clock::time_point time);

    ~Frame();

    cv::Mat GetFrame() const;

    int GetFrameNum() const;

    std::chrono::system_clock::time_point GetFrameTime() const;

    std::vector<cv::Rect> GetBoxes() const;

    void SetBoxes(const std::vector<cv::Rect> boxes);
};
