#define _CRT_SECURE_NO_WARNINGS
#include "YoloDetection.h"
#include <unordered_set>

YoloDetection::YoloDetection()
{
	nlohmann::json configJson;
	JsonManager::CheckIfJsonModified(configJson);
#ifdef _WIN32
	const std::string os = "_windows";
#elif __linux__
	const std::string os = "_linux";
#endif
	const std::filesystem::path netPath = configJson["yolo_settings"]["net_path" + os];
	if (!std::filesystem::exists(netPath)) {
		throw std::runtime_error("Network path does not exist: " + netPath.string());
	}
	const std::filesystem::path classListPath = configJson["yolo_settings"]["class_list_path" + os];
	if (!std::filesystem::exists(classListPath)) {
		throw std::runtime_error("Class list path does not exist: " + classListPath.string());
	}
	net = cv::dnn::readNet(netPath.string());

	net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

	std::lock_guard<std::mutex> lock(yoloMutex);


	inputWidth = configJson["yolo_settings"]["input_width"];
	inputHeight = configJson["yolo_settings"]["input_height"];
	confidenceThreshold = configJson["yolo_settings"]["confidence_threshold"];
	scoreThreshold = configJson["yolo_settings"]["score_threshold"];
	nmsThreshold = configJson["yolo_settings"]["nms_threshold"];

	std::ifstream ifs(classListPath.string());
	std::string line;
	if (!ifs.is_open())
		throw std::runtime_error("Error opening 'class list' file for yolov5");

	while (getline(ifs, line))
	{
		classList.push_back(line);
	}
}



std::vector<cv::Mat> YoloDetection::PreProcess(const cv::Mat& inputImage)
{
	if (inputImage.empty())
		throw std::runtime_error("empty image");
	cv::Mat blob;
	LOG_TRACE("before blobFromImage.");
	cv::dnn::blobFromImage(inputImage, blob, 1. / 255., cv::Size(inputWidth, inputHeight), cv::Scalar(), true, false);
	LOG_DEBUG("after blobFromImage.");
	net.setInput(blob);

	std::vector<cv::Mat> outputs;
	LOG_TRACE("before net.forward.");
	net.forward(outputs, net.getUnconnectedOutLayersNames());
	LOG_DEBUG("after net.forward.");
	return outputs;
}


cv::Mat YoloDetection::post_process(cv::Mat& input_image, std::vector<cv::Mat>& outputs, std::unordered_set<std::string>& classes) {
	/*classes.insert("person");
	classes.insert("cell phone"); */
	// Initialize vectors to hold respective outputs while unwrapping detections.
	std::vector<int> class_ids;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	// Resizing factor.
	float x_factor = input_image.cols / INPUT_WIDTH;
	float y_factor = input_image.rows / INPUT_HEIGHT;
	float* data = (float*)outputs[0].data;
	const int dimensions = 85;
	// 25200 for default size 640.
	const int rows = 25200;
	// Iterate through 25200 detections.
	for (int i = 0; i < rows; ++i) {
		float confidence = data[4];
		// Discard bad detections and continue.
		if (confidence >= CONFIDENCE_THRESHOLD) {
			float* classes_scores = data + 5;
			// Create a 1x85 Mat and store class scores of 80 classes.
			cv::Mat scores(1, classList.size(), CV_32FC1, classes_scores);
			// Perform minMaxLoc and acquire the index of best class score.
			cv::Point class_id;
			double max_class_score;
			cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
			// Continue if the class score is above the threshold.
			if (max_class_score > SCORE_THRESHOLD && classes.find(classList[class_id.x]) != classes.end()) {
				// Store class ID and confidence in the pre-defined respective vectors.
				confidences.push_back(confidence);
				class_ids.push_back(class_id.x);
				// Center.
				float cx = data[0];
				float cy = data[1];
				// Box dimension.
				float w = data[2];
				float h = data[3];
				// Bounding box coordinates.
				int left = int((cx - 0.5 * w) * x_factor);
				int top = int((cy - 0.5 * h) * y_factor);
				int width = int(w * x_factor);
				int height = int(h * y_factor);
				// Store good detections in the boxes vector.
				boxes.push_back(cv::Rect(left, top, width, height));
			}
		}
		// Jump to the next row.
		data += dimensions;
	}

	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
	for (int i = 0; i < indices.size(); i++) {
		int idx = indices[i];
		cv::Rect box = boxes[idx];
		int left = box.x;
		int top = box.y;
		int width = box.width;
		int height = box.height;
		// Draw bounding box.
		cv::rectangle(input_image, cv::Point(left, top),
			cv::Point(left + width, top + height), cv::Scalar(255, 0, 0),
			3 * THICKNESS);
		// Get the label for the class name and its confidence.
		std::string label = cv::format("%.2f", confidences[idx]);
		label = classList[class_ids[idx]] + ":" + label;
		// Draw class labels.
		draw_label(input_image, label, left, top);
	}
	return input_image;
}

void YoloDetection::draw_label(cv::Mat& input_image, std::string label, int left, int top) {
	// Display the label at the top of the bounding box.
	int baseLine;
	cv::Size label_size =
		cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	top = std::max(top, label_size.height);
	// Top left corner.
	cv::Point tlc = cv::Point(left, top);
	// Bottom right corner.
	cv::Point brc =
		cv::Point(left + label_size.width, top + label_size.height + baseLine);
	// Draw white rectangle.
	cv::rectangle(input_image, tlc, brc, cv::Scalar(255, 255, 255), cv::FILLED);
	// Put the label on the black rectangle.
	cv::putText(input_image, label, cv::Point(left, top + label_size.height),
		cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
}


