#pragma once
#include <QObject>
#include "ServerAPI.h"

class ServerAPIController : public QObject {
    Q_OBJECT
public:
    explicit ServerAPIController(ServerAPI* api) : serverAPI(api) {}

public slots:
    void startServer() {
        serverAPI->StartServer();
    }

    void stopServer() {
        serverAPI->StopServer();
    }

    bool isServerRunning() {
        return serverAPI->IsServerRunning();
    }
    void pushEmptyFrame(cv::Mat frame){
        serverAPI->GetFrameShowQueue()->Push(std::make_shared<FrameWithDetails>(frame, -1, std::chrono::system_clock::now()));
    }
    // Add other slots as needed to control the server

    // Add the polling function to continuously check for frames
    void pollFramesForDisplay() {
        while (true) {
            if (serverAPI->GetFramePtocessQueue()->TryPop(frameToShow)) {
                // Process and emit the frameReady signal with the frame
                emit frameReady(frameToShow->GetFrame());
            }
            // Add a delay to avoid busy-waiting (e.g., sleep for a few milliseconds)
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

signals:
    void frameReady(const cv::Mat& frame); // Signal to indicate a frame is ready for display

private:
    ServerAPI* serverAPI;
    std::shared_ptr<FrameWithDetails> frameToShow; // Declare frameToShow here or as a member variable
};
