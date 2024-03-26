#pragma once
#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>
#include "CaptureAndPreprocess.h"


class CameraController
{
private:
    std::shared_ptr<CaptureAndPreprocess> captureAndPreprocess;

public:
	CameraController();
	~CameraController();
    grpc::Status StartCamera(grpc::ServerContext* context, const google::protobuf::Empty* request, google::protobuf::Empty* response) {
        captureAndPreprocess->StartRunCamera();
        return grpc::Status::OK;
    }

    grpc::Status StopCamera(grpc::ServerContext* context, const google::protobuf::Empty* request, google::protobuf::Empty* response) {
        captureAndPreprocess->StopRunCamera();
        return grpc::Status::OK;
    }
};

CameraController::CameraController()
{
}

CameraController::~CameraController()
{
}