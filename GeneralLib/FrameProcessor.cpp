#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include "FrameProcessor.h"

#define NUM_DETECTION_THREADS 1

FrameProcessor::FrameProcessor() :isServerRunning(false){
    service = std::make_shared<FrameReceiver>();
    JsonManager::CheckIfJsonModified(configJson);
#ifdef _WIN32
    std::string os = "_windows";
#elif __linux__
    std::string os = "_linux";
#endif
    std::string path = configJson["output_settings"]["video_path" + os];
    videoPath = path;
    path = configJson["output_settings"]["db_path" + os];
    dbPath = path;
    path = configJson["output_settings"]["image_path" + os];
    imagePath = path;
    mDBManager.CreateDB(dbPath.string());
    InitializeVideoWriter();
}

void FrameProcessor::InitializeVideoWriter() {
    LOG_DEBUG("Creating video writer...");
    mVideoWriter.open(videoPath.string(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 3, cv::Size(640, 480));

    if (!mVideoWriter.isOpened()) {
        throw std::runtime_error("Failed to open video writer.");
    }

    LOG_DEBUG("Video writer initialized.");
}

void FrameProcessor::StartServer() {
    StartFrameReceiverThread();
    isServerRunning = true;
    frameReceiverThread.detach();
}

void FrameProcessor::StartFrameProcessing() {
    InitializeTable();
    CreateImageDirectory();

    for (int i = 0; i < NUM_DETECTION_THREADS; ++i) {
        StartFrameProcessingThread(i);
    }
}

void FrameProcessor::ShutdownServer() {
    ReleaseVideoWriter();
    server->Shutdown();
    LOG_INFO("Server shutdown.");
}

void FrameProcessor::InitializeTable() {
    LOG_INFO("Creating database table...");
    mDBManager.CreateTable();
    LOG_INFO("Database table created.");
}

void FrameProcessor::CreateImageDirectory() {
    // Get the current date
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);
    std::tm currentTm = *std::localtime(&inTimeT);

    // Create a folder for the current date (YYYY-MM-DD format)
    std::stringstream dateSs;
    dateSs << std::put_time(&currentTm, "%Y-%m-%d");
    std::filesystem::path dateFolder = imagePath / dateSs.str();

    // Create a subfolder with the current time (HH-MM-SS format)
    std::stringstream timeSs;
    timeSs << std::put_time(&currentTm, "%H-%M-%S");
    std::filesystem::path timeSubfolder = dateFolder / timeSs.str();

    // Create the date folder if it doesn't exist
    if (!std::filesystem::exists(dateFolder)) {
        std::filesystem::create_directory(dateFolder);
    }

    // Create the time subfolder if it doesn't exist
    if (!std::filesystem::exists(timeSubfolder)) {
        std::filesystem::create_directory(timeSubfolder);
    }

    // Set the imagesDirectory to the time subfolder
    imagesDirectory = timeSubfolder;
}

void FrameProcessor::StartFrameReceiverThread() {
    LOG_DEBUG("Starting frame receiver thread...");
    frameReceiverThread = std::thread(&FrameProcessor::RunFrameReceiver, this);
}

void FrameProcessor::StartFrameProcessingThread(int threadIndex) {
    LOG_DEBUG("Starting Frame Processing thread {}...", threadIndex);

    frameProcessingThreads.emplace_back([this, threadIndex]() {
        LOG_DEBUG("Thread {} starting its work.", threadIndex);
        YoloDetection yoloDetection;
        this->ProcessFrame(yoloDetection, threadIndex);
        LOG_DEBUG("Thread {} finished processing frames.", threadIndex);
        });
}

void FrameProcessor::StopRunServer() {
    isServerRunning = false;
}

void FrameProcessor::WaitForFrameProcessingThreadsToFinish() {
    LOG_DEBUG("Waiting for all frame processing threads to finish...");
    for (auto& thread : frameProcessingThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    LOG_INFO("Processing completed.");
}

void FrameProcessor::ReleaseVideoWriter() {
    LOG_INFO("Releasing video writer...");
    mVideoWriter.release();
    LOG_INFO("Video writer released.");
}

void FrameProcessor::ProcessFrame(YoloDetection& yoloDetection, int threadIndex) {
    LOG_DEBUG("Thread {} processing frame...", threadIndex);
    

    while (isServerRunning)
    {
        auto isFreeToDetect = service->IsFreeToDetect();
        if (!isFreeToDetect->load()) {
            auto frame = service->GetFrameToDetect();
            auto processStartTime = std::chrono::high_resolution_clock::now();
            LOG_DEBUG("Thread {} popped frame {} from frameProcessQueue.", threadIndex, frame->GetFrameNum());
            DetectFrame(frame, yoloDetection);
            isFreeToDetect->store(true);

            mVideoWriter.write(frame->GetFrame());
            SaveFrameAsImage(frame->GetFrame(), frame->GetFrameNum());

            for (auto& box : frame->GetBoxes()) {
                auto avgRGB = CalculateAverageRGB(box, frame->GetFrame());
                mDBManager.InsertData(frame->GetFrameNum(), box, avgRGB, frame->GetFrameTime());
            }
            LOG_DEBUG("Thread {} finished processing frame {} after {} ms.", threadIndex, frame->GetFrameNum(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - processStartTime));
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
    }
}

void FrameProcessor::DetectFrame(std::shared_ptr<FrameWithDetails> frame, YoloDetection& yoloDetection) {
    auto detections = yoloDetection.PreProcess(frame->GetFrame());
    frame->SetBoxes(yoloDetection.PostProcess(frame->GetFrame(), detections));
}

cv::Scalar FrameProcessor::CalculateAverageRGB(const cv::Rect& box, const cv::Mat& frame) {
    cv::Mat subImg = frame(box);
    cv::Scalar avgColor = cv::mean(subImg);
    return avgColor;
}

void FrameProcessor::SaveFrameAsImage(const cv::Mat& frame, const int frameNum) {
    std::string imageName = "frame_" + std::to_string(frameNum) + ".png";
    cv::imwrite(imagesDirectory.string() + "/" + imageName, frame);
}

void FrameProcessor::RunFrameReceiver() {
    JsonManager::CheckIfJsonModified(configJson);
    std::string ip = configJson["grpc_settings"]["server_ip_address"];
    std::string port = configJson["grpc_settings"]["port_number"];
    std::string serverAddress(ip + ":" + port);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());

    LOG_INFO("Server connecting...");
    server = builder.BuildAndStart();
    LOG_INFO("Server connected.");
    LOG_INFO("Server listening on {}", serverAddress);

    server->Wait();
}

std::shared_ptr<FrameReceiver> FrameProcessor::GetService() {
    return service;
}