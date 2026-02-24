#pragma once

#include <qqmlintegration.h>

#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>

class ClockImageProvider;
class Renderer;

class ClockBridge : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString imageSource READ imageSource NOTIFY imageSourceChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

   public:
    explicit ClockBridge(QObject* parent = nullptr);
    ~ClockBridge() override;

    QString imageSource() const;
    bool ready() const;
    QString errorString() const;

    Q_INVOKABLE bool initialize(const QUrl& bg, const QUrl& hour,
                                const QUrl& minute);
    Q_INVOKABLE void refreshNow();

   signals:
    void imageSourceChanged();
    void readyChanged();
    void errorStringChanged();

   private slots:
    void onUpdateTimerTimeout();

   private:
    bool ensureImageProvider();
    bool renderForCurrentTime();
    void scheduleNextMinuteTick();
    void setReady(bool value);
    void setErrorString(const QString& value);
    QString toLocalPath(const QUrl& url) const;
    QString buildImageSource() const;
    void bumpFrameRevision();

    Renderer* renderer_ = nullptr;
    ClockImageProvider* imageProvider_ = nullptr;
    QString providerId_;
    QString imageSource_;
    QString errorString_;
    bool ready_ = false;
    bool initialized_ = false;
    int frameRevision_ = 0;
    QTimer updateTimer_;
};
