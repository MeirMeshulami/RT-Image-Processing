#include "Frame.h"

Frame::Frame() {}

Frame::Frame(const cv::Mat frame, const int frameNum, const std::chrono::system_clock::time_point time)
    : frame(frame), frameNum(frameNum), time(time) {}


Frame::Frame(Frame&& other)noexcept
    : frame(std::move(other.frame)),
    frameNum(other.frameNum),
    time(other.time),
    boxes(std::move(other.boxes))
{
}


Frame& Frame::operator=(Frame&& other) noexcept
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

Frame::~Frame() {}

cv::Mat Frame::GetFrame() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return frame;
}

int Frame::GetFrameNum() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return frameNum;
}

std::chrono::system_clock::time_point Frame::GetFrameTime() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return time;
}

std::vector<cv::Rect> Frame::GetBoxes() const
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during read access
    return boxes;
}

void Frame::SetBoxes(const std::vector<cv::Rect> boxes)
{
    std::lock_guard<std::mutex> lock(mutex_); // Lock during write access
    this->boxes = boxes;
}
