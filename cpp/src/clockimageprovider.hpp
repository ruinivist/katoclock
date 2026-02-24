#pragma once

#include <QImage>
#include <QMutex>
#include <QQuickImageProvider>

class ClockImageProvider : public QQuickImageProvider {
   public:
    ClockImageProvider();

    QImage requestImage(const QString& id, QSize* size,
                        const QSize& requestedSize) override;

    void setImage(const QImage& image);

   private:
    QMutex mutex_;
    QImage image_;
};
