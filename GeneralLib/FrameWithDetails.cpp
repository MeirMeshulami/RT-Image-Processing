#include "FrameWithDetails.h"

FrameWithDetails::FrameWithDetails() {}

FrameWithDetails::FrameWithDetails(const cv::Mat frame, const int frameNum, const std::chrono::system_clock::time_point time)
    : frame(frame), frameNum(frameNum), time(time) {}

// Implement the move constructor
FrameWithDetails::FrameWithDetails(FrameWithDetails&& other)noexcept
    : frame(std::move(other.frame)),
    frameNum(other.frameNum),
    time(other.time),
    boxes(std::move(other.boxes))
{
}

// Implement the move assignment operator
FrameWithDetails& FrameWithDetails::operator=(FrameWithDetails&& other) noexcept
{
    if (this != &other)
    {
        frame = std::move(other.frame);
        frameNum = other.frameNum;
        time = other.time;
        boxes = std::move(other.boxes);
    }
    return *this;
}

FrameWithDetails::~FrameWithDetails() {}

cv::Mat FrameWithDetails::GetFrame() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return frame;
}

int FrameWithDetails::GetFrameNum() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return frameNum;
}

std::chrono::system_clock::time_point FrameWithDetails::GetFrameTime() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return time;
}

std::vector<cv::Rect> FrameWithDetails::GetBoxes() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return boxes;
}

void FrameWithDetails::SetBoxes(const std::vector<cv::Rect> boxes)
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during write access
    this->boxes = boxes;
}
