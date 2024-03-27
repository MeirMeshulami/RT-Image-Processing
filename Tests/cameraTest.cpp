#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include "CaptureAndPreprocess.h"

using namespace ::testing;

TEST(CalculateSimilarity, SameMatrix) {
	// arrange
	int data[3][3] = { {1,1,1},{2,2,2},{3,3,3} };
	cv::Mat prev(3, 3, CV_32F, data);
	cv::Mat curr = prev.clone();
	
	// act
	int result = CaptureAndPreprocess::CalculateSimilarity(curr, prev);

	// assert
	EXPECT_EQ(0, result);
}


