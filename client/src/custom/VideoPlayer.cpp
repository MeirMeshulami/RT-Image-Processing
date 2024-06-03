#include "VideoPlayer.h"
#include <QFileDialog>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    mediaPlayer = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);
    audioOutput= new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setRange(0, 0);
    connect(positionSlider, &QSlider::sliderMoved, this, &VideoPlayer::setPosition);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80); // Set default volume
    volumeSlider->setFixedSize(100, 20);
    connect(volumeSlider, &QSlider::sliderMoved, this, &VideoPlayer::onUpdateVolume);
    audioButton=new QPushButton(this);
    audioButton->setIcon(QIcon(":/feather/audio.png"));
    audioButton->setCursor(Qt::PointingHandCursor);
    connect(audioButton, &QPushButton::clicked, this, &VideoPlayer::toggleMute);


    playButton = new QPushButton(this);
    playButton->setIcon(QIcon(":/feather/play.png"));
    connect(playButton, &QPushButton::clicked, this, &VideoPlayer::play);
    playButton->setIconSize(QSize(50, 50));
    playButton->setCursor(Qt::PointingHandCursor);

    openButton = new QPushButton(this);
    openButton->setIcon(QIcon(":/feather/browse.png"));
    connect(openButton, &QPushButton::clicked, this, &VideoPlayer::openFile);
    openButton->setIconSize(QSize(50, 50));
    openButton->setCursor(Qt::PointingHandCursor);

    QPushButton *prevButton = new QPushButton(this);
    prevButton->setIcon(QIcon(":/feather/prev.png"));
    connect(prevButton, &QPushButton::clicked, this, &VideoPlayer::prev);
    prevButton->setIconSize(QSize(50, 50));
    prevButton->setCursor(Qt::PointingHandCursor);

    QPushButton *nextButton = new QPushButton(this);
    nextButton->setIcon(QIcon(":/feather/next.png"));
    connect(nextButton, &QPushButton::clicked, this, &VideoPlayer::next);
    nextButton->setIconSize(QSize(50, 50));
    nextButton->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(volumeSlider);
    buttonLayout->addWidget(audioButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(openButton);

    // Create a vertical layout for the video widget, position slider, and buttons
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    layout->addWidget(positionSlider);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    // Add hover effect using stylesheet
    QString buttonStyle = "QPushButton:hover {"
                          " background-color: rgba(255, 255, 255, 50);" // Change the alpha value to adjust the shine effect
                          " border: none;"
                          "}";

    playButton->setStyleSheet(buttonStyle);
    openButton->setStyleSheet(buttonStyle);
    prevButton->setStyleSheet(buttonStyle);
    nextButton->setStyleSheet(buttonStyle);

    mediaPlayer->setVideoOutput(videoWidget);

    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::updatePosition);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        positionSlider->setRange(0, int(duration));
    });
}

VideoPlayer::~VideoPlayer()
{
    delete mediaPlayer;
    delete videoWidget;
    delete playButton;
    delete positionSlider;
    delete openButton;
}

void VideoPlayer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"), QDir::homePath());
    if (!fileName.isEmpty()) {
        mediaPlayer->setSource(QUrl::fromLocalFile(fileName));
        playButton->setEnabled(true);
        play();
        QString directoryPath = QFileInfo(fileName).path();
        loadMediaFromDirectory(directoryPath);
    }
}

void VideoPlayer::play()
{
    if (!mediaPlayer->mediaStatus()) {
        QMessageBox::warning(this, "No Video", "No video is opened.");
        return;
    }
    switch (mediaPlayer->playbackState()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer->pause();
        playButton->setIcon(QIcon(":/feather/play.png"));
        break;
    default:
        mediaPlayer->play();
        playButton->setIcon(QIcon(":/feather/pause.png"));
        break;
    }
}

void VideoPlayer::updatePosition(qint64 position)
{
    positionSlider->setValue(position);
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer->setPosition(position);
}

void VideoPlayer::next(){
    if(!mediaSources.isEmpty()) {
        // Move to the next media source
        currentMediaIndex = (currentMediaIndex + 1) % mediaSources.size();
        mediaPlayer->setSource(QUrl::fromLocalFile(mediaSources[currentMediaIndex]));
        mediaPlayer->play();
    }
}

void VideoPlayer::prev(){
    if(!mediaSources.isEmpty()) {
        // Move to the previous media source
        currentMediaIndex = (currentMediaIndex - 1 + mediaSources.size()) % mediaSources.size();
        mediaPlayer->setSource(QUrl::fromLocalFile(mediaSources[currentMediaIndex]));
        mediaPlayer->play();
    }
}

void VideoPlayer::loadMediaFromDirectory(const QString &directoryPath) {
    // Clear existing media sources
    mediaSources.clear();

    // Create a QDir object for the directory
    QDir directory(directoryPath);

    // Filter for files only
    QStringList filters;
    filters << "*.mp4" << "*.avi" << "*.mkv"; // Add more file extensions as needed
    directory.setNameFilters(filters);

    // Get the list of files in the directory
    QStringList files = directory.entryList(QDir::Files);

    // Iterate through the list of files and add them to the media sources
    for(const QString &file : files) {
        QString filePath = directory.filePath(file);
        mediaSources.append(filePath);
    }
}

void VideoPlayer::onUpdateVolume(int volume)
{
    audioOutput->setVolume(volume/ 100.0);
}

void VideoPlayer::toggleMute(){
    if(audioOutput->isMuted()){
        audioOutput->setMuted(false);
        audioButton->setIcon(QIcon(":/feather/audio.png"));
    }else{
        audioOutput->setMuted(true);
        audioButton->setIcon(QIcon(":/feather/audio-mute.png"));
    }
}
