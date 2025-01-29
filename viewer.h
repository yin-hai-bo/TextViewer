#ifndef VIEWER_H
#define VIEWER_H

#include <QTextBrowser>

class Viewer : public QTextBrowser
{
    Q_OBJECT
public:
    explicit Viewer(QWidget * parent = nullptr);
    void documentOpened() { documentOpened_ = true; }
    void documentClosed()
    {
        documentOpened_ = false;
        autoScrollState_ = false;
    }

signals:

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    bool documentOpened_ = false;

    bool autoScrollState_ = false;
    QPoint autoScrollAnchor_;
    QPoint mouseMiddleButtonDownPosition_;
};

#endif // VIEWER_H
