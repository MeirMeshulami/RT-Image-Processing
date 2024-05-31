#pragma once
#include "service.grpc.pb.h"
#include "Settings.h"
#include <grpcpp/grpcpp.h>
#include <opencv2/opencv.hpp>

class ServerService final : public FrameService::Service {
private:
	cv::VideoCapture cam;
	std::atomic<bool> isSettingsChanged;
	int MotionDetector(const cv::Mat& currentFrame, const cv::Mat& previousFrame, double threshold);
	grpc::Status GetFrame(grpc::ServerContext* context, const FrameRequest* request, grpc::ServerWriter<FrameResponse>* writer)override;
	grpc::Status UpdateConfigurations(grpc::ServerContext* context, const UpdateConfig* request, ConfigAck* response);

public:
	nlohmann::json configs;

	ServerService();
	void RunServer();
};
