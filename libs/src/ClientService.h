#pragma once
#include "Frame.h"
#include "LogManager.h"
#include "service.grpc.pb.h"
#include "service.pb.h"
#include "ThreadSafeQueue.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <json.hpp>
#include <memory>
#include <opencv2/opencv.hpp>


class ClientService {
public:
	ClientService();
	void Connect(std::string serverAddress);
	void GetFrames();
	void PushFrameIntoQueue(std::shared_ptr<Frame> frame);
	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> GetFrameShowQueue();
	void DestroyConnection();
	void StartStreamFrames();
	void StopStreamFrames();
	bool UpdateServerSettings(nlohmann::json& configs);

	std::atomic<bool> stopStreaming;
	std::atomic<bool> isConnect;

private:
	std::shared_ptr<grpc::Channel> channel;
	std::unique_ptr<FrameService::Stub> stub;
	std::shared_ptr<ThreadSafeQueue<std::shared_ptr<Frame>>> frameShowQueue;
};