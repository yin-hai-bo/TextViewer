#include "viewer.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

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
    if (documentOpened_ && autoScrollState_) {
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
            autoScrollState_ = !autoScrollState_;
            if (autoScrollState_) {
                autoScrollAnchor_ = event->pos();
            }
            this->repaint();
        }
    }
    QTextBrowser::mouseReleaseEvent(event);
}
