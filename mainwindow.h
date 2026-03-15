#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTranslator>
#include <memory>
#include "config.h"
#include "recentlist.h"
#include "viewer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QFile;

class MainWindow : public QMainWindow, public RecentList::IRecentFileSignalReceiver
{
    Q_OBJECT

public:
    using FileValidator = QString (*)(const QFile & file);

    MainWindow(QWidget *parent = nullptr);
    MainWindow(QSettings::Format format,
               QSettings::Scope scope,
               const QString & organization,
               const QString & application,
               QWidget *parent = nullptr);
    ~MainWindow();

    void init();
    bool openFileForTesting(const QString & filename);
    bool openFileForTesting(const QString & filename, FileValidator validator);

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

    void onActionLanguageTriggered();

    void onAutoScrollingChanged(bool onOff, Viewer::Direction dir);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QTranslator> translator_;

    std::array<QLabel *, 3> statusBarLabels_ = {};

    QString fileName_;
    size_t fileSize_ = 0;

    bool autoScrolling_ = false;
    Viewer::Direction autoScrollDirection_ = Viewer::Direction::None;

    Config config_;
    std::unique_ptr<RecentList> recentList_;

    void initWindowState();
    void initStatusBar();
    void initTextBrowser();
    void initRecentFilesMenu();
    void initLanguageMenu();
    void setLanguage(Config::Language language);

    void changeLanguage(const QStringList & localeList);
    void resetLanguage();

    void addFileToRecents(const QString & filename);
    bool openFile(const QString & filename);
    bool openFile(const QString & filename, FileValidator validator);
    static QString validateFile(const QFile & file);

    void refreshStatusBar();
    void refreshStatusBarAutoScrollState();

    bool isFileOpened() const { return !fileName_.isEmpty(); }
};
#endif // MAINWINDOW_H
