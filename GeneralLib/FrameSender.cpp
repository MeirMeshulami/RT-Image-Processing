#include "FrameSender.h"

FrameSender::FrameSender(std::shared_ptr<grpc::Channel> channel): stub_(FrameService::NewStub(channel)) {}

bool FrameSender::SendFrame(const cv::Mat& frame, const int frameNum, const std::chrono::system_clock::time_point frameTime) {
	FrameMessage imageFrame;
	imageFrame.set_frame_number(frameNum);

    google::protobuf::Timestamp* timestamp = imageFrame.mutable_frame_time();
    timestamp->set_seconds(std::chrono::time_point_cast<std::chrono::seconds>(frameTime).time_since_epoch().count());
    timestamp->set_nanos(static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(frameTime.time_since_epoch() % std::chrono::seconds(1)).count()));

    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);
    imageFrame.set_image_data(buffer.data(), buffer.size());

	grpc::ClientContext context;
	google::protobuf::Empty response;
	auto status = stub_->SendFrame(&context, imageFrame, &response);

	if (status.ok())
		return true;	
	return false;
}