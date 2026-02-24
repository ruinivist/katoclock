#include "clockimageprovider.hpp"

#include <QMutexLocker>

ClockImageProvider::ClockImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage ClockImageProvider::requestImage(const QString&, QSize* size,
                                        const QSize& requestedSize) {
    QImage frame;
    {
        QMutexLocker locker(&mutex_);
        frame = image_;
    }

    if (size != nullptr) {
        *size = frame.size();
    }

    if (!frame.isNull() && requestedSize.width() > 0 &&
        requestedSize.height() > 0) {
        return frame.scaled(requestedSize, Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);
    }

    return frame;
}

void ClockImageProvider::setImage(const QImage& image) {
    QMutexLocker locker(&mutex_);
    image_ = image;
}
