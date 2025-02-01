#ifndef VIEWER_H
#define VIEWER_H

#include <QTextBrowser>
#include <QTimer>
#include "utils.h"

class Viewer : public QTextBrowser
{
    Q_OBJECT
public:
    enum class Direction { None, Up, Down };

    explicit Viewer(QWidget * parent = nullptr);
    void documentOpened() { documentOpened_ = true; }
    void documentClosed()
    {
        documentOpened_ = false;
        stopAutoScroll();
    }

    bool isAutoScrollState() const { return timer_; }

signals:
    void autoScrollStateChangeSignal(bool onOff, Direction direction);

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

private slots:
    void onTimerTriggered();

private:
    QPixmap imgAnchor_;
    QPixmap imgUp_;
    QPixmap imgDown_;

    QPixmap const * currentCursor_ = nullptr;

    bool documentOpened_ = false;

    QPoint autoScrollAnchor_;
    QPoint mouseMiddleButtonDownPosition_;
    int autoScrollSpeed_ = 0;

    Connectable<QTimer> timer_;

    void startAutoScroll();
    void stopAutoScroll();

    void changeCursor(const QPixmap &);

    int calcAutoScrollSpeed(QPoint mouseCurrentPos);
    const QPixmap & decideCursorInAutoScrolling() const;
};

#endif // VIEWER_H
