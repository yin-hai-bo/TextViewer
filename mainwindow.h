#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "config.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const MainWindow &) = delete;
    MainWindow & operator=(const MainWindow &) = delete;
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionClose_triggered();

    void on_actionFont_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_actionRestoreDefaultZoom_triggered();

private:
    Ui::MainWindow * ui;
    Config config_;

    QLabel * statusBarLabel_TotalLength_ = nullptr;

    void initStatusBar();
    void initTextBrowser();
};
#endif // MAINWINDOW_H
