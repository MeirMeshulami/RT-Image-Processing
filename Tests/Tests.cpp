#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <gmock/gmock.h>
#include <unordered_set>

#include "FrameWithDetails.h"
#include "CaptureAndPreprocess.h"
#include "FrameProcessor.h"
#include "JsonManager.h" 

using namespace ::testing;

#pragma region ThreadSafeQueueTest
TEST(ThreadSafeQueueTest, BasicPushPop) {
	ThreadSafeQueue<int> queue;

	queue.Push(1);

	int value;
	auto success = queue.TryPop(value);

	EXPECT_TRUE(success);
	EXPECT_EQ(value, 1);
	EXPECT_TRUE(queue.Empty());
}

// Test concurrent push and pop
TEST(ThreadSafeQueueTest, ConcurrentPushPop) {
	ThreadSafeQueue<int> queue;

	constexpr int numIterations = 1000;
	constexpr int numThreads = 4;

	// Create threads to push
	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back([&queue, i, numIterations]() {
			for (int j = 0; j < numIterations; ++j) {
				int value = i * numIterations + j;
				queue.Push(value);
			}
			});
	}

	// Wait for threads to finish
	for (auto& thread : threads) {
		thread.join();
	}

	// Collect pushed values
	std::unordered_set<int> pushedValues;
	for (int i = 0; i < numThreads * numIterations; ++i) {
		pushedValues.insert(i);
	}

	// Pop values and check against pushed values
	int value;
	while (queue.TryPop(value)) {
		auto it = pushedValues.find(value);
		ASSERT_NE(it, pushedValues.end());  // Ensure the value was pushed
		pushedValues.erase(it);             // Remove the value from the set
	}

	// Check if all values were pushed and popped
	EXPECT_TRUE(pushedValues.empty());
	EXPECT_TRUE(queue.Empty());
}

TEST(ThreadSafeQueueTest, EmptyPop) {
	ThreadSafeQueue<int> queue;
	int value = 42;
	bool success = queue.TryPop(value);
	EXPECT_FALSE(success);
	EXPECT_EQ(value, 42); // Value should remain unchanged
}

TEST(ThreadSafeQueueTest, MultipleProducersConsumers) {
	ThreadSafeQueue<int> queue;

	constexpr int numProducers = 4;
	constexpr int numConsumers = 4;
	constexpr int numIterationsPerThread = 1000;

	std::vector<std::thread> producers;
	std::vector<std::thread> consumers;

	// Set to keep track of values produced and consumed
	std::unordered_set<int> pushedValues;
	std::unordered_set<int> consumedValues;
	std::mutex pushedValuesMutex;
	std::mutex consumedValuesMutex;

	// Create producer threads
	for (int i = 0; i < numProducers; ++i) {
		producers.emplace_back([&queue, &pushedValues, &pushedValuesMutex, numIterationsPerThread, i]() {
			for (int j = 0; j < numIterationsPerThread; ++j) {
				int value = i * numIterationsPerThread + j;
				queue.Push(value);

				// Lock and add value to pushedValues
				std::lock_guard<std::mutex> lock(pushedValuesMutex);
				pushedValues.insert(value);
			}
			queue.Push(-1);
			});
	}
	int braker = 0;
	// Create consumer threads
	for (int i = 0; i < numConsumers; ++i) {
		consumers.emplace_back([&queue, &consumedValues, &consumedValuesMutex, &braker]() {
			int value;
			while (braker != -4) {
				if (queue.TryPop(value)) {
					if (value == -1) {
						braker += value;
						continue;
					}
					// Lock and add value to consumedValues
					std::lock_guard<std::mutex> lock(consumedValuesMutex);
					consumedValues.insert(value);
				}
			}
			});
	}

	// Wait for all producer threads to finish
	for (auto& thread : producers) {
		thread.join();
	}

	// Wait for all consumer threads to finish
	for (auto& thread : consumers) {
		thread.join();
	}

	std::lock_guard<std::mutex> pushedLock(pushedValuesMutex);
	std::lock_guard<std::mutex> consumedLock(consumedValuesMutex);
	for (int value : pushedValues) {
		auto it = consumedValues.find(value);
		ASSERT_NE(it, consumedValues.end());  // Ensure the value was consumed
		consumedValues.erase(it);             // Remove the value from the set
	}

	// Verify that the queue is empty
	EXPECT_TRUE(queue.Empty());
}

TEST(ThreadSafeQueueTest, PushPopOrder) {
	ThreadSafeQueue<int> queue;

	constexpr int numIterations = 1000;

	for (int i = 0; i < numIterations; ++i) {
		queue.Push(i);
	}

	for (int i = 0; i < numIterations; ++i) {
		int value;
		bool success = queue.TryPop(value);
		EXPECT_TRUE(success);
		EXPECT_EQ(value, i);
	}

	EXPECT_TRUE(queue.Empty());
}
#pragma endregion

#pragma region YoloDetectionTest

TEST(YoloDetectionTest, PreProcessEmptyInputImage) {
	YoloDetection detector;
	cv::Mat emptyImage;
	EXPECT_THROW(detector.PreProcess(emptyImage), std::runtime_error);
}

TEST(YoloDetectionTest, PostProcessEmptyInputImage) {
	YoloDetection detector;
	cv::Mat emptyImage;
	std::vector<cv::Mat> outputs; // Create empty outputs
	EXPECT_THROW(detector.PostProcess(emptyImage, outputs), std::runtime_error);
}
TEST(YoloDetectionTest, PostProcessEmptyOutputs) {
	YoloDetection detector;
	cv::Mat inputImage(480, 640, CV_8UC3); // Create a non-empty image
	std::vector<cv::Mat> emptyOutputs; // Empty outputs
	EXPECT_THROW(detector.PostProcess(inputImage, emptyOutputs), std::runtime_error);
}

#pragma endregion

#pragma region CaptureAndPreprocessTest

#pragma endregion

int main(int argc, char** argv) {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
	testing::InitGoogleTest(&argc, argv);

	RUN_ALL_TESTS();

	cv::waitKey();
	return 0;
}



