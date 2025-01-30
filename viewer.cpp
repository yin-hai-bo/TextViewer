#include "viewer.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>

static QPixmap * s_scrollAnchorPicture;
static constexpr int SCROLL_ANCHOR_WIDTH = 32;
static constexpr int SCROLL_ANCHOR_HEIGHT = 32;

Viewer::Viewer(QWidget * parent)
    : QTextBrowser{parent}
{
    if (s_scrollAnchorPicture == nullptr) {
        s_scrollAnchorPicture = new QPixmap(":/images/scroll-flag.png");
    }
}

void Viewer::paintEvent(QPaintEvent * event)
{
    QTextBrowser::paintEvent(event);
    if (documentOpened_ && isAutoScrollState()) {
        QPainter painter(viewport());
        auto pos = autoScrollAnchor_;
        pos.rx() -= SCROLL_ANCHOR_WIDTH / 2;
        pos.ry() -= SCROLL_ANCHOR_HEIGHT / 2;
        painter.drawPixmap(pos, *s_scrollAnchorPicture);
    }
}

void Viewer::mousePressEvent(QMouseEvent * event)
{
    if (documentOpened_ && event->button() == Qt::MiddleButton) {
        mouseMiddleButtonDownPosition_ = event->pos();
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
                autoScrollSpeed_ = 0.f;
                timer_.reset(new QTimer());
                timer_.connect(&QTimer::timeout, this, &Viewer::onTimerTriggered);
                timer_->start(50);
            }
            this->repaint();
        }
    }
    QTextBrowser::mouseReleaseEvent(event);
}

void Viewer::mouseMoveEvent(QMouseEvent * event)
{
    QTextBrowser::mouseMoveEvent(event);
    if (isAutoScrollState()) {
        autoScrollSpeed_ = (event->pos().y() - autoScrollAnchor_.y()) / 50.f;
    }
}

void Viewer::onTimerTriggered()
{
    auto const bar = this->verticalScrollBar();
    bar->setValue(bar->value() + autoScrollSpeed_);
}

void Viewer::stopAutoScroll()
{
    timer_.reset();
}
