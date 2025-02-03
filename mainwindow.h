#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTranslator>
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

    void init();

    void onActionRecentFile(int index, const QString & filename) override;

protected:
    void changeEvent(QEvent * event) override;
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

    void on_actionLineHeight_triggered();

    void on_actionLanguageTriggered();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QTranslator> translator_;

    std::array<QLabel *, 3> statusBarLabels_ = {};

    Config config_;
    std::unique_ptr<RecentList> recentList_;

    struct AutoScrollState
    {
        bool enabled;
        QPoint anchor;
    } autoScrollState_;

    void initWindowState();
    void initStatusBar();
    void initTextBrowser();
    void initRecentFilesMenu();
    void initLanguageMenu();
    void initLanguage();

    void changeLanguage(const QStringList & localeList);
    void resetLanguage();

    void addFileToRecents(const QString & filename);
    bool openFile(const QString & filename);

};
#endif // MAINWINDOW_H
