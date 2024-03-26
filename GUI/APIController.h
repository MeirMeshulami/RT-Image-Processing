#pragma once
#include <QObject>
#include "API.h"

class APIController : public QObject {
    Q_OBJECT
public:
    explicit APIController(API* api) : api(api) {}

public slots:
    void startServer() {
        api->StartServer();
    }

    void stopServer() {
        api->StopServer();
    }

    bool isServerRunning() {
        return api->IsServerRunning();
    }
    void pushEmptyFrame(cv::Mat frame) {
        api->GetFrameShowQueue()->Push(std::make_shared<FrameWithDetails>(frame, -1, std::chrono::system_clock::now()));
    }

    void pollFramesForDisplay() {
        while (true) {
            if (!api->IsProcessingFrames()) {
                if (api->GetFrameShowQueue()->TryPop(frameToShow)) {
                    emit frameReady(frameToShow->GetFrame());
                }
            }
            else {
                if (api->GetFramePtocessQueue()->TryPop(frameToShow)) {
                    emit frameReady(frameToShow->GetFrame());
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }


signals:
    void frameReady(const cv::Mat& frame); // Signal to indicate a frame is ready for display

private:
    API* api;
    std::shared_ptr<FrameWithDetails> frameToShow; // Declare frameToShow here or as a member variable
};
