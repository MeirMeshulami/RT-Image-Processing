#include "YoloDetection.h"

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
	const std::filesystem::path classListPath = configJson["yolo_settings"]["class_list_path" + os];
	net = cv::dnn::readNetFromONNX(netPath.string());

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
	// Convert to blob.
	cv::Mat blob;
	LOG_TRACE("before blobFromImage.");
	cv::dnn::blobFromImage(inputImage, blob, 1. / 255., cv::Size(inputWidth, inputHeight), cv::Scalar(), true, false);
	LOG_DEBUG("after blobFromImage.");
	net.setInput(blob);

	// Forward propagate.
	std::vector<cv::Mat> outputs;
	LOG_TRACE("before net.forward.");
	net.forward(outputs, net.getUnconnectedOutLayersNames());
	LOG_DEBUG("after net.forward.");
	return outputs;
}


std::vector<cv::Rect> YoloDetection::PostProcess(const cv::Mat &inputImage, const std::vector<cv::Mat>& outputs)
{
	if (inputImage.empty())
		throw std::runtime_error("empty image");
	if (outputs.empty())
		throw std::runtime_error("empty outputs");

	// Initialize vectors to hold respective outputs while unwrapping detections.
	YoloDetection frameDetections;
	// Resizing factor.
	auto xFactor = (float)inputImage.cols / inputWidth;
	auto yFactor = (float)inputImage.rows / inputHeight;
	auto* data = (float*)outputs[0].data;
	const auto dimensions = 85;
	// 25200 for default size 640.
	const auto rows = 25200;
	// Iterate through 25200 detections.
	for (int i = 0; i < rows; ++i)
	{
		auto confidence = data[4];
		// Discard bad detections and continue.
		if (confidence >= confidenceThreshold)
		{
			auto* classesScores = data + 5;
			// Create a 1x85 Mat and store class scores of 80 classes.
			cv::Mat scores(1, (int)classList.size(), CV_32FC1, classesScores);
			// Perform minMaxLoc and acquire the index of best class  score.
			cv::Point classId;
			double maxClassScore;
			cv::minMaxLoc(scores, 0, &maxClassScore, 0, &classId);
			// Continue if the class score is above the threshold.
			if (maxClassScore > scoreThreshold)// && class_id.x == 2
			{
				// Store class ID and confidence in the pre-defined respective vectors.
				frameDetections.confidences.push_back(confidence);
				frameDetections.classIds.push_back(classId.x);
				// Center.
				auto cx = data[0];
				auto cy = data[1];
				// Box dimension.
				auto w = data[2];
				auto h = data[3];
				// Bounding box coordinates.
				auto left = int((cx - 0.5 * w) * xFactor);
				if (left < 0)
					left = 0;
				auto top = int((cy - 0.5 * h) * yFactor);
				if (top < 0) 
					top = 0;
				auto width = int(w * xFactor);
				if ((left + width) > inputImage.cols)
					width = inputImage.cols - left;
				auto height = int(h * yFactor);
				if ((top + height) > inputImage.rows)
					height = inputImage.rows - top;
				// Store good detections in the boxes vector.
				frameDetections.boxes.push_back(cv::Rect(left, top, width, height));
			}
		}
		// Jump to the next row.
		data += 85;
	}
	// Perform Non-Maximum Suppression and draw predictions.
	std::vector<int> indices;
	cv::dnn::NMSBoxes(frameDetections.boxes, frameDetections.confidences, scoreThreshold, scoreThreshold, indices);

	std::vector<cv::Rect> detectedBoxes; 

	for (int i = 0; i < indices.size(); i++)
	{
		int idx = indices[i];
		auto box = frameDetections.boxes[idx];

		//pushes each box in vector that function returns.
		detectedBoxes.push_back(box);

		DrawBoundingBox(inputImage, box, frameDetections.confidences[idx], frameDetections.classIds[idx]);
	}
	return detectedBoxes;
}

void YoloDetection::DrawBoundingBox(const cv::Mat& frame, const cv::Rect& box, const float confidence, const int classId) {
	auto left = box.x;
	auto top = box.y;
	auto width = box.width;
	auto height = box.height;

	// Draw bounding box
	rectangle(frame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, 3 * THICKNESS);

	// Get the label for the class name and its confidence
	auto label = cv::format("%.2f", confidence);
	label = classList[classId] + ":" + label;

	// Draw class labels
	DrawLabel(frame, label, left, top);
}


void YoloDetection::DrawLabel(const cv::Mat& inputImage, const std::string label, const int left, int top)
{
	// Display the label at the top of the bounding box.
	int baseLine;
	auto labelSize = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
	top = std::max(top, labelSize.height);
	// Top left corner.
	auto tlc = cv::Point(left, top);
	// Bottom right corner.
	auto brc = cv::Point(left + labelSize.width, top + labelSize.height + baseLine);
	// Draw white rectangle.
	rectangle(inputImage, tlc, brc, BLACK, cv::FILLED);
	// Put the label on the black rectangle.
	putText(inputImage, label, cv::Point(left, top + labelSize.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS);
}