#pragma once
#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>
#include "LogManager.h"
#include "FrameWithDetails.h"
#include "ThreadSafeQueue.h"

enum QueueType {
    SHOW = 0,
    PROCESS = 1
};

class FrameReceiver final : public FrameService::Service {
private:
    std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> frameProcessQueue;
    std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> frameShowQueue;
    std::shared_ptr<FrameWithDetails> frameToDetect;
    std::shared_ptr<std::atomic<bool>> isFreeToDetect;
    std::atomic<bool> isShowingFrames;
    std::atomic<bool> isProcessingFrames;

public:
    FrameReceiver();

    grpc::Status SendFrame(grpc::ServerContext* context, const FrameMessage* request, google::protobuf::Empty* response);

    void PushFrameIntoQueue(std::shared_ptr<FrameWithDetails> frame, QueueType type);

    std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> GetFrameProcessQueue();

    std::shared_ptr<ThreadSafeQueue<std::shared_ptr<FrameWithDetails>>> GetFrameShowQueue();

    std::shared_ptr<FrameWithDetails> GetFrameToDetect();

    std::shared_ptr<std::atomic<bool>> IsFreeToDetect();

    void ControlShowingFrames();

    void ControlProcessingFrames();
};
