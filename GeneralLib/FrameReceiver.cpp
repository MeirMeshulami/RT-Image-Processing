#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include "FrameReceiver.h"

FrameReceiver::FrameReceiver()
    :isShowingFrames(true), isProcessingFrames(true) {
    frameProcessQueue = std::make_shared<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>>();
    frameShowQueue = std::make_shared<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>>();
    isFreeToDetect = std::make_shared<std::atomic<bool>>(true);
}

grpc::Status FrameReceiver::SendFrame(grpc::ServerContext* context, const FrameMessage* request, google::protobuf::Empty* response) {
    LOG_DEBUG("Received data from camera.");

    const auto& imageData = request->image_data();
    std::vector<uchar> buffer(imageData.begin(), imageData.end());
    auto receivedFrame = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (receivedFrame.empty()) {
        LOG_INFO("receivedFrame empty");

        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Error: Failed to decode the received image.");
    }
    else {
        auto frameTimePoint = std::chrono::system_clock::time_point(std::chrono::seconds(request->frame_time().seconds()) + std::chrono::milliseconds(request->frame_time().nanos() / 1000000));

        auto timeT = std::chrono::system_clock::to_time_t(frameTimePoint);
        std::tm* timeInfo = std::localtime(&timeT);
        if (timeInfo == nullptr)
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Error converting time.");
        else
        {
            LOG_INFO("Received frame {} with timestamp {}", request->frame_number(), frameTimePoint);
            PushFrameIntoQueue(std::make_shared<FrameWithDetails>(receivedFrame, request->frame_number(), frameTimePoint), SHOW);
            LOG_INFO("Frame {} pushed into frameShowQueue.", request->frame_number());
            auto currentFrame = std::make_shared<FrameWithDetails>(receivedFrame, request->frame_number(), frameTimePoint);
            if (isFreeToDetect) {
                frameToDetect = currentFrame;
                isFreeToDetect = false;
            }
            else
            {
                cv::Mat grayscaleFrame;
                cv::cvtColor(currentFrame->GetFrame(), grayscaleFrame, cv::COLOR_BGR2GRAY);
                cv::Mat bgrGrayscaleFrame;
                cv::cvtColor(grayscaleFrame, bgrGrayscaleFrame, cv::COLOR_GRAY2BGR);
                cv::addWeighted(currentFrame->GetFrame(), 0.4, bgrGrayscaleFrame, 0.6, 0, currentFrame->GetFrame());

            }
            PushFrameIntoQueue(currentFrame, PROCESS);
        }
    }
    return grpc::Status(grpc::StatusCode::OK, "Success.");
}

void FrameReceiver::PushFrameIntoQueue(std::shared_ptr<FrameWithDetails> frame, QueueType type) {
    bool isRunning{};
    std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> queue;
    if (type == SHOW)
    {
        isRunning = isShowingFrames;
        queue = frameShowQueue;
    }
    else if(type == PROCESS)
    {
        isRunning = isProcessingFrames;
        queue = frameProcessQueue;
    }
    if (isRunning)
    {
        if (queue->GetQueueSize() > 30)
        {
            std::shared_ptr<FrameWithDetails> toThrowOut;
            queue->TryPop(toThrowOut);
            LOG_ERROR("The frameProcessQueue reached its maximum and popped out frame {}.", toThrowOut->GetFrameNum());
        }
        queue->Push(frame);
    }
}

std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> FrameReceiver::GetFrameProcessQueue() {
    return frameProcessQueue;

}

std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> FrameReceiver::GetFrameShowQueue() {
    return frameShowQueue;
}

std::shared_ptr<FrameWithDetails> FrameReceiver::GetFrameToDetect() {
    return frameToDetect;
}

std::shared_ptr<std::atomic<bool>> FrameReceiver::IsFreeToDetect()
{
    return isFreeToDetect;
}

void FrameReceiver::ControlShowingFrames() {
    isShowingFrames = !isShowingFrames;
    if(isShowingFrames)
        LOG_INFO("Showing video started.");
    else
        LOG_INFO("Showing video stoped.");
}

void FrameReceiver::ControlProcessingFrames() {
    isProcessingFrames = !isProcessingFrames;
    if (isProcessingFrames)
        LOG_INFO("Processing frames started.");
    else
        LOG_INFO("Processing frames stoped.");
}

