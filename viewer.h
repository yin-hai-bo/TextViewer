#ifndef VIEWER_H
#define VIEWER_H

#include <QTextBrowser>
#include <QTimer>
#include "utils.h"

class Viewer : public QTextBrowser
{
    Q_OBJECT
public:
    explicit Viewer(QWidget * parent = nullptr);
    void documentOpened() { documentOpened_ = true; }
    void documentClosed()
    {
        documentOpened_ = false;
        stopAutoScroll();
    }

    bool isAutoScrollState() const { return timer_; }

signals:

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

private slots:
    void onTimerTriggered();

private:
    bool documentOpened_ = false;

    QPoint autoScrollAnchor_;
    QPoint mouseMiddleButtonDownPosition_;
    float autoScrollSpeed_ = 0.f;

    Connectable<QTimer> timer_;

    void stopAutoScroll();
};

#endif // VIEWER_H
