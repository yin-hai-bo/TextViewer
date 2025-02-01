#include "viewer.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QScrollBar>

static constexpr int AUTO_SCROLL_THRESHOLD = 48;

Viewer::Viewer(QWidget * parent)
    : QTextBrowser(parent)
    , imgAnchor_(":/images/scroll-flag.png")
    , imgUp_(":/images/up.png")
    , imgDown_(":/images/down.png")
{}

void Viewer::mousePressEvent(QMouseEvent * event)
{
    if (documentOpened_) {
        auto const button = event->button();
        if (Qt::MiddleButton == button) {
            mouseMiddleButtonDownPosition_ = event->pos();
        } else {
            stopAutoScroll();
        }
    }
    QTextBrowser::mousePressEvent(event);
}

void Viewer::mouseReleaseEvent(QMouseEvent * event)
{
    if (documentOpened_ && event->button() == Qt::MiddleButton) {
        auto delta = event->pos() - mouseMiddleButtonDownPosition_;
        auto distance = delta.x() * delta.x() + delta.y() * delta.y();
        if (distance <= 16) {
            if (isAutoScrollState()) {
                stopAutoScroll();
            } else {
                autoScrollAnchor_ = event->pos();
                autoScrollSpeed_ = 0;
                startAutoScroll();
            }
            this->repaint();
        }
    }
    QTextBrowser::mouseReleaseEvent(event);
}

int Viewer::calcAutoScrollSpeed(QPoint mouseCurrentPos)
{
    int delta = mouseCurrentPos.y() - autoScrollAnchor_.y();
    int absDelta = abs(delta);

    if (absDelta > AUTO_SCROLL_THRESHOLD) {
        absDelta -= AUTO_SCROLL_THRESHOLD;
        absDelta /= 15;
        if (absDelta < 50) {
            autoScrollSpeed_ = absDelta;
        } else {
            autoScrollSpeed_ = absDelta * absDelta;
        }
        if (delta < 0) {
            autoScrollSpeed_ = -autoScrollSpeed_;
        } else {
        }
    } else {
        autoScrollSpeed_ = 0;
    }

    return autoScrollSpeed_;
}

const QPixmap & Viewer::decideCursorInAutoScrolling() const
{
    if (autoScrollSpeed_ < 0) {
        return imgUp_;
    }
    return autoScrollSpeed_ > 0 ? imgDown_ : imgAnchor_;
}

static Viewer::Direction decideDirection(int autoScrollSpeed)
{
    if (autoScrollSpeed < 0) {
        return Viewer::Direction::Down;
    }
    return autoScrollSpeed > 0 ? Viewer::Direction::Up : Viewer::Direction::None;
}

void Viewer::mouseMoveEvent(QMouseEvent * event)
{
    QTextBrowser::mouseMoveEvent(event);
    if (isAutoScrollState()) {
        calcAutoScrollSpeed(event->pos());
        auto & expectedCursor = decideCursorInAutoScrolling();
        if (&expectedCursor != currentCursor_) {
            changeCursor(expectedCursor);
            emit autoScrollStateChangeSignal(true, decideDirection(autoScrollSpeed_));
        }
    }
}

void Viewer::onTimerTriggered()
{
    auto const bar = this->verticalScrollBar();
    bar->setValue(bar->value() + autoScrollSpeed_);
}

void Viewer::startAutoScroll()
{
    assert(!timer_);
    timer_.reset(new QTimer());
    timer_.connect(&QTimer::timeout, this, &Viewer::onTimerTriggered);
    timer_->start(50);

    changeCursor(imgAnchor_);

    emit autoScrollStateChangeSignal(true, Direction::None);
}

void Viewer::stopAutoScroll()
{
    timer_.reset();
    this->viewport()->setCursor(Qt::ArrowCursor);
    emit autoScrollStateChangeSignal(false, Direction::None);
}

void Viewer::changeCursor(const QPixmap & pixmap)
{
    this->viewport()->setCursor(QCursor(pixmap, pixmap.width() / 2, pixmap.height() / 2));
    currentCursor_ = &pixmap;
}
