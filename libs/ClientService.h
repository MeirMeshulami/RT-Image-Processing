#pragma once
#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include <opencv2/opencv.hpp>
#include "service.pb.h"
#include "service.grpc.pb.h"
#include "LogManager.h"
#include "ThreadSafeQueue.h"
#include "Frame.h"


enum QueueType {
	SHOW = 0,
	PROCESS = 1
};

class ClientService {
public:

	ClientService(std::shared_ptr<grpc::Channel> channel);

	void GetFrame();

	bool UpdateConfigurations(const std::string& file);

	void PushFrameIntoQueue(std::shared_ptr<Frame> frame, QueueType type);

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameProcessQueue();

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue();

	std::shared_ptr<Frame> GetFrameToDetect();

	std::shared_ptr<std::atomic<bool>> IsFreeToDetect();

	void ControlShowingFrames();

	void ControlProcessingFrames();

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> frameProcessQueue;
	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> frameShowQueue;
	std::unique_ptr<FrameService::Stub> stub_;
	std::atomic<bool> stopStreaming;

private:
	
	
	std::shared_ptr<Frame> frameToDetect;
	std::shared_ptr<std::atomic<bool>> isFreeToDetect;
	std::atomic<bool> isShowingFrames;
	std::atomic<bool> isProcessingFrames;
};