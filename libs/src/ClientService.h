#pragma once
#include "Frame.h"
#include "LogManager.h"
#include "service.grpc.pb.h"
#include "service.pb.h"
#include "ThreadSafeQueue.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>



class ClientService {
public:

	ClientService();

	void Connect(std::string serverAddress);

	void GetFrame();

	bool UpdateConfigs(const std::string& file);

	void PushFrameIntoQueue(std::shared_ptr<Frame> frame);

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue();

	std::shared_ptr<Frame> GetFrameToDetect();

	void DestroyConnection();

	void StartStreamFrames();

	void StopStreamFrames();

	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> frameShowQueue;
	std::unique_ptr<FrameService::Stub> stub_;
	std::atomic<bool> stopStreaming;
	std::atomic<bool> isConnect;

private:
	std::shared_ptr<Frame> frameToDetect;
	std::shared_ptr<grpc::Channel> channel;
};