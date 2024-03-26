#pragma once
#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>
#include <opencv2/opencv.hpp>

class FrameSender {
private:
    std::unique_ptr<FrameService::Stub> stub_;

public:
    FrameSender(std::shared_ptr<grpc::Channel> channel);

    bool SendFrame(const cv::Mat& frame, const int frameNum, const std::chrono::system_clock::time_point frameTime);
};