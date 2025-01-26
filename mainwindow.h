#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <memory>
#include "config.h"
#include "recentlist.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public RecentList::IRecentFileSignalReceiver
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init(const QApplication &);

    void onActionRecentFile(int index, const QString & filename) override;

protected:
    void closeEvent(QCloseEvent * event) override;

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionClose_triggered();

    void on_actionFont_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_actionRestoreDefaultZoom_triggered();

    void on_actionAbout_triggered();

private:
    std::unique_ptr<Ui::MainWindow> ui;

    QLabel * statusBarLabel_TotalLength_ = nullptr;

    Config config_;
    std::unique_ptr<RecentList> recentList_;

    void initWindowState(const QApplication &);
    void initStatusBar();
    void initTextBrowser();
    void initRecentFilesMenu();

    void addFileToRecents(const QString & filename);
    bool openFile(const QString & filename);

};
#endif // MAINWINDOW_H
