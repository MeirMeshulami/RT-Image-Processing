#pragma once
#include "JsonManager.h"
#include "LogManager.h"
#include "service.grpc.pb.h"
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <json.hpp>
#include <opencv2/opencv.hpp>

class ServerService final : public FrameService::Service {
private:
	cv::VideoCapture cam;

public:
	nlohmann::json configs;

	ServerService();

	void RunServer();

	grpc::Status GetFrame(grpc::ServerContext* context, const FrameRequest* request, grpc::ServerWriter<FrameResponse>* writer)override;

	grpc::Status UpdateConfigurations(grpc::ServerContext* context, const UpdateConfig* request, ConfigAck* response);

	int MotionDetector(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold);

};
