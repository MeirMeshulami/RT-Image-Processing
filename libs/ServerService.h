#pragma once
#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include <opencv2/opencv.hpp>

#include "service.grpc.pb.h"
#include "JsonManager.h"
#include "LogManager.h"

class ServerService final : public FrameService::Service {
private:
	cv::VideoCapture cam;
	std::atomic<bool> isCameraRunning;
	nlohmann::json configJson;
	bool isModified;
public:
	ServerService();

	grpc::Status GetFrame(grpc::ServerContext* context, const FrameRequest* request, grpc::ServerWriter<FrameResponse>* writer)override;

	grpc::Status UpdateConfigurations(grpc::ServerContext* context, const UpdateConfig* request, ConfigAck* response);

	void RunServer();

	int CalculateSimilarity(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold);

	void StopRunCamera();

	std::atomic<bool>& IsCameraRunning();

};
