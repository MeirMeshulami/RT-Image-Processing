#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QPushButton>
#include <QSlider>
#include <QBoxLayout>
#include <QAudioOutput>
#include <QMessageBox>

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

private slots:
    void openFile();
    void play();
    void updatePosition(qint64 position);
    void setPosition(int position);
    void next();
    void prev();
    void onUpdateVolume(int volume);
    void toggleMute();

private:
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    QVideoWidget* videoWidget;
    QPushButton* playButton;
    QSlider* positionSlider;
    QSlider* volumeSlider;
    QPushButton* openButton;
    QPushButton *audioButton;
    QStringList mediaSources;
    int currentMediaIndex = 0;

    void loadMediaFromDirectory(const QString &directoryPath);

};

#endif // VIDEOPLAYER_H
