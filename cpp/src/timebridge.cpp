#include "timebridge.h"

#include <QQmlEngine>
#include <QTime>
#include <QUuid>
#include <opencv2/imgproc.hpp>

#include "clockimageprovider.hpp"
#include "renderer.hpp"

namespace {
constexpr int kMillisPerMinute = 60 * 1000;
}

ClockBridge::ClockBridge(QObject* parent) : QObject(parent) {
    updateTimer_.setSingleShot(true);
    connect(&updateTimer_, &QTimer::timeout, this,
            &ClockBridge::onUpdateTimerTimeout);
}

ClockBridge::~ClockBridge() {
    updateTimer_.stop();
    if (renderer_ != nullptr) {
        delete renderer_;
        renderer_ = nullptr;
    }
}

QString ClockBridge::imageSource() const { return imageSource_; }

bool ClockBridge::ready() const { return ready_; }

QString ClockBridge::errorString() const { return errorString_; }

bool ClockBridge::initialize(const QUrl& bg, const QUrl& hour,
                             const QUrl& minute) {
    updateTimer_.stop();
    initialized_ = false;
    setReady(false);

    const QString bgPath = toLocalPath(bg);
    const QString hourPath = toLocalPath(hour);
    const QString minutePath = toLocalPath(minute);
    if (bgPath.isEmpty() || hourPath.isEmpty() || minutePath.isEmpty()) {
        setErrorString(QStringLiteral("Clock assets must be local file URLs."));
        return false;
    }

    if (!ensureImageProvider()) {
        return false;
    }

    if (renderer_ == nullptr) {
        renderer_ = new Renderer();
    }

    if (!renderer_->load_assets(bgPath.toStdString(), hourPath.toStdString(),
                                minutePath.toStdString())) {
        setErrorString(QStringLiteral("Failed to load clock assets."));
        return false;
    }

    initialized_ = true;
    if (!renderForCurrentTime()) {
        initialized_ = false;
        return false;
    }

    scheduleNextMinuteTick();
    return true;
}

void ClockBridge::refreshNow() {
    if (!initialized_) {
        setErrorString(QStringLiteral("ClockBridge is not initialized."));
        setReady(false);
        return;
    }

    renderForCurrentTime();
    scheduleNextMinuteTick();
}

void ClockBridge::onUpdateTimerTimeout() {
    if (!initialized_) {
        return;
    }

    renderForCurrentTime();
    scheduleNextMinuteTick();
}

bool ClockBridge::ensureImageProvider() {
    if (imageProvider_ != nullptr) {
        return true;
    }

    QQmlEngine* engine = qmlEngine(this);
    if (engine == nullptr) {
        setErrorString(QStringLiteral("ClockBridge has no active QQmlEngine."));
        return false;
    }

    providerId_ = QStringLiteral("catclock_%1")
                      .arg(QUuid::createUuid().toString(QUuid::Id128));
    if (engine->imageProvider(providerId_) != nullptr) {
        setErrorString(QStringLiteral("Image provider ID collision."));
        return false;
    }

    imageProvider_ = new ClockImageProvider();
    engine->addImageProvider(providerId_, imageProvider_);
    return true;
}

bool ClockBridge::renderForCurrentTime() {
    if (renderer_ == nullptr || imageProvider_ == nullptr) {
        setErrorString(QStringLiteral("ClockBridge is not initialized."));
        setReady(false);
        return false;
    }

    const QTime now = QTime::currentTime();
    const int minute = now.minute();
    const int hour = now.hour();

    const double minuteAngle = minute * 6.0;
    const double hourAngle = (hour % 12) * 30.0 + minute * 0.5;

    cv::Mat frame = renderer_->render(hourAngle, minuteAngle);
    if (frame.empty()) {
        setErrorString(QStringLiteral("Renderer returned an empty frame."));
        setReady(false);
        return false;
    }

    cv::Mat rgbaFrame;
    cv::cvtColor(frame, rgbaFrame, cv::COLOR_BGRA2RGBA);
    QImage image(rgbaFrame.data, rgbaFrame.cols, rgbaFrame.rows,
                 static_cast<int>(rgbaFrame.step), QImage::Format_RGBA8888);
    imageProvider_->setImage(image.copy());

    setErrorString(QString());
    setReady(true);
    bumpFrameRevision();
    return true;
}

void ClockBridge::scheduleNextMinuteTick() {
    if (!initialized_) {
        return;
    }

    const QTime now = QTime::currentTime();
    int delayMs = kMillisPerMinute - (now.second() * 1000 + now.msec());
    if (delayMs <= 0 || delayMs > kMillisPerMinute) {
        delayMs = kMillisPerMinute;
    }

    updateTimer_.start(delayMs);
}

void ClockBridge::setReady(bool value) {
    if (ready_ == value) {
        return;
    }
    ready_ = value;
    emit readyChanged();
}

void ClockBridge::setErrorString(const QString& value) {
    if (errorString_ == value) {
        return;
    }
    errorString_ = value;
    emit errorStringChanged();
}

QString ClockBridge::toLocalPath(const QUrl& url) const {
    if (url.isEmpty()) {
        return QString();
    }

    if (url.isLocalFile()) {
        return url.toLocalFile();
    }

    if (url.scheme().isEmpty()) {
        return url.toString();
    }

    return QString();
}

QString ClockBridge::buildImageSource() const {
    if (providerId_.isEmpty()) {
        return QString();
    }

    return QStringLiteral("image://%1/frame?rev=%2")
        .arg(providerId_)
        .arg(frameRevision_);
}

// to signal image inddeed changed even if the path is same
// we update revision
void ClockBridge::bumpFrameRevision() {
    ++frameRevision_;
    const QString nextSource = buildImageSource();
    if (imageSource_ == nextSource) {
        return;
    }

    imageSource_ = nextSource;
    emit imageSourceChanged();
}
