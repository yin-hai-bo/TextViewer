#include <QtTest>
#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QMenu>
#include <QStatusBar>
#include <QTemporaryDir>
#include <QUuid>

#include "config.h"
#include "mainwindow.h"
#include "viewer.h"

namespace
{
QTemporaryDir & settingsRoot()
{
    static QTemporaryDir DIR;
    return DIR;
}

void configureTestSettingsBackend()
{
    static bool CONFIGURED = false;
    if (CONFIGURED) {
        return;
    }

    QVERIFY2(settingsRoot().isValid(), "Failed to create temporary settings directory");
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsRoot().path());
    CONFIGURED = true;
}

QString uniqueName(const char * suffix)
{
    return QString("TextViewerMainWindowTests_%1_%2")
        .arg(suffix)
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

Config testConfig(const QString & application)
{
    configureTestSettingsBackend();
    return Config(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
}

QString createTestFile(const char * suffix, const QString & content)
{
    configureTestSettingsBackend();
    QString const path = QDir(QDir::currentPath())
                             .filePath("main_window_test_files/" + uniqueName(suffix) + ".txt");
    QDir().mkpath(QFileInfo(path).absolutePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to create test file" << path << file.errorString();
        return QString();
    }

    if (file.write(content.toUtf8()) < 0 || !file.flush()) {
        qWarning() << "Failed to write test file" << path << file.errorString();
        return QString();
    }

    return path;
}

QString createTestFilePath(const char * suffix)
{
    configureTestSettingsBackend();
    QString const path = QDir(QDir::currentPath())
                             .filePath("main_window_test_files/" + uniqueName(suffix) + ".txt");
    QDir().mkpath(QFileInfo(path).absolutePath());
    return path;
}

QList<QAction *> recentFileActions(QMenu & menu)
{
    QList<QAction *> actions;
    for (QAction * action : menu.actions()) {
        if (action != nullptr && action->text().contains(" - ")) {
            actions.push_back(action);
        }
    }
    return actions;
}

QString statusBarDocumentText(const MainWindow & window)
{
    auto * statusBar = window.findChild<QStatusBar *>("statusbar");
    if (statusBar == nullptr) {
        return QString();
    }

    for (QLabel * label : statusBar->findChildren<QLabel *>()) {
        QString const text = label->text();
        if (text == QString("(No document)") || text.startsWith("Text size: ")) {
            return text;
        }
    }

    return QString();
}

QString openFileAndCaptureWarning(MainWindow & window, const QString & path)
{
    QString warningText;

    QTimer::singleShot(0, [&warningText]() {
        auto * widget = QApplication::activeModalWidget();
        auto * messageBox = qobject_cast<QMessageBox *>(widget);
        QVERIFY(messageBox != nullptr);
        warningText = messageBox->text();
        messageBox->accept();
    });

    bool const opened = window.openFileForTesting(path);
    if (opened) {
        qWarning() << "Expected open failure for" << path;
    }
    if (!opened) {
        return warningText;
    }
    return warningText;
}

QString openFileAndCaptureWarning(
    MainWindow & window,
    const QString & path,
    MainWindow::FileValidator validator)
{
    QString warningText;

    QTimer::singleShot(0, [&warningText]() {
        auto * widget = QApplication::activeModalWidget();
        auto * messageBox = qobject_cast<QMessageBox *>(widget);
        QVERIFY(messageBox != nullptr);
        warningText = messageBox->text();
        messageBox->accept();
    });

    bool const opened = window.openFileForTesting(path, validator);
    if (opened) {
        qWarning() << "Expected open failure for" << path;
    }
    if (!opened) {
        return warningText;
    }
    return warningText;
}

QString rejectAsTooLarge(const QFile &)
{
    return QString("Too large file.");
}
}

class MainWindowTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init_startsWithClosedDocumentState();
    void init_selectsSystemLanguageByDefault();
    void openFile_loadsNormalTextAndUpdatesWindowState();
    void openFile_rejectsEmptyFile();
    void openFile_rejectsMissingFile();
    void openFile_usesInjectedValidator();
    void openFile_updatesRecentMenuAndDeduplicates();
    void recentMenuAction_reopensFileAndPromotesEntry();
    void closeEvent_persistsWindowGeometry();
};

void MainWindowTest::initTestCase()
{
    configureTestSettingsBackend();
}

void MainWindowTest::init_startsWithClosedDocumentState()
{
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", uniqueName("empty_state"));
    window.init();
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    auto * closeAction = window.findChild<QAction *>("actionClose");
    auto * openAction = window.findChild<QAction *>("actionOpen");
    auto * viewer = window.findChild<Viewer *>("textBrowser");
    auto * statusBar = window.findChild<QStatusBar *>("statusbar");

    QVERIFY(closeAction != nullptr);
    QVERIFY(openAction != nullptr);
    QVERIFY(viewer != nullptr);
    QVERIFY(statusBar != nullptr);

    QCOMPARE(window.windowTitle(), QString("Text Viewer"));
    QVERIFY(openAction->isEnabled());
    QVERIFY(!closeAction->isEnabled());
    QVERIFY(viewer->toPlainText().isEmpty());

    bool foundEmptyState = false;
    for (QLabel * label : statusBar->findChildren<QLabel *>()) {
        if (label->text() == QString("(No document)")) {
            foundEmptyState = true;
            break;
        }
    }
    QVERIFY(foundEmptyState);
}

void MainWindowTest::init_selectsSystemLanguageByDefault()
{
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", uniqueName("language"));
    window.init();

    auto * systemAction = window.findChild<QAction *>("actionLanguageSystem");
    auto * englishAction = window.findChild<QAction *>("actionLanguageEnglish");
    auto * chineseAction = window.findChild<QAction *>("actionLanguageSimplifiedChinese");

    QVERIFY(systemAction != nullptr);
    QVERIFY(englishAction != nullptr);
    QVERIFY(chineseAction != nullptr);

    QVERIFY(systemAction->isChecked());
    QVERIFY(!englishAction->isChecked());
    QVERIFY(!chineseAction->isChecked());
}

void MainWindowTest::openFile_loadsNormalTextAndUpdatesWindowState()
{
    QString const path = createTestFile("sample", "hello\nworld");
    QVERIFY(!path.isEmpty());

    QString const application = uniqueName("open_success");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    auto * viewer = window.findChild<Viewer *>("textBrowser");
    auto * closeAction = window.findChild<QAction *>("actionClose");
    QVERIFY(viewer != nullptr);
    QVERIFY(closeAction != nullptr);

    QVERIFY(window.openFileForTesting(path));

    QCOMPARE(viewer->toPlainText(), QString("hello\nworld"));
    QCOMPARE(window.windowTitle(), QString("Text Viewer - [%1]").arg(QFileInfo(path).fileName()));
    QVERIFY(closeAction->isEnabled());
    QCOMPARE(statusBarDocumentText(window), QString("Text size: 12"));

    Config reloaded = testConfig(application);
    QStringList const expectedRecentFiles{path};
    QCOMPARE(reloaded.recentFiles(), expectedRecentFiles);
}

void MainWindowTest::openFile_rejectsEmptyFile()
{
    QString const path = createTestFile("empty", "");
    QVERIFY(!path.isEmpty());

    QString const application = uniqueName("open_empty");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    auto * viewer = window.findChild<Viewer *>("textBrowser");
    auto * closeAction = window.findChild<QAction *>("actionClose");
    QVERIFY(viewer != nullptr);
    QVERIFY(closeAction != nullptr);

    QCOMPARE(openFileAndCaptureWarning(window, path), QString("It's an empty file."));
    QVERIFY(viewer->toPlainText().isEmpty());
    QCOMPARE(window.windowTitle(), QString("Text Viewer"));
    QVERIFY(!closeAction->isEnabled());
    QCOMPARE(statusBarDocumentText(window), QString("(No document)"));

    Config reloaded = testConfig(application);
    QVERIFY(reloaded.recentFiles().isEmpty());
}

void MainWindowTest::openFile_rejectsMissingFile()
{
    QString const path = createTestFilePath("missing");
    QString const application = uniqueName("open_missing");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    auto * viewer = window.findChild<Viewer *>("textBrowser");
    auto * closeAction = window.findChild<QAction *>("actionClose");
    QVERIFY(viewer != nullptr);
    QVERIFY(closeAction != nullptr);

    QCOMPARE(openFileAndCaptureWarning(window, path), QString("Open file failed."));
    QVERIFY(viewer->toPlainText().isEmpty());
    QCOMPARE(window.windowTitle(), QString("Text Viewer"));
    QVERIFY(!closeAction->isEnabled());
    QCOMPARE(statusBarDocumentText(window), QString("(No document)"));

    Config reloaded = testConfig(application);
    QVERIFY(reloaded.recentFiles().isEmpty());
}

void MainWindowTest::openFile_usesInjectedValidator()
{
    QString const path = createTestFile("validator", "small");
    QVERIFY(!path.isEmpty());

    QString const application = uniqueName("open_validator");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    auto * viewer = window.findChild<Viewer *>("textBrowser");
    auto * closeAction = window.findChild<QAction *>("actionClose");
    QVERIFY(viewer != nullptr);
    QVERIFY(closeAction != nullptr);

    QCOMPARE(openFileAndCaptureWarning(window, path, &rejectAsTooLarge), QString("Too large file."));
    QVERIFY(viewer->toPlainText().isEmpty());
    QCOMPARE(window.windowTitle(), QString("Text Viewer"));
    QVERIFY(!closeAction->isEnabled());
    QCOMPARE(statusBarDocumentText(window), QString("(No document)"));

    Config reloaded = testConfig(application);
    QVERIFY(reloaded.recentFiles().isEmpty());
}

void MainWindowTest::openFile_updatesRecentMenuAndDeduplicates()
{
    QString const firstPath = createTestFile("first", "first");
    QString const secondPath = createTestFile("second", "second");
    QVERIFY(!firstPath.isEmpty());
    QVERIFY(!secondPath.isEmpty());

    QString const application = uniqueName("recent_dedup");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    QVERIFY(window.openFileForTesting(firstPath));
    QVERIFY(window.openFileForTesting(secondPath));
    QVERIFY(window.openFileForTesting(firstPath));

    auto * menu = window.findChild<QMenu *>("menu_File");
    QVERIFY(menu != nullptr);

    QList<QAction *> const actions = recentFileActions(*menu);
    QCOMPARE(actions.size(), 2);
    QCOMPARE(actions[0]->text(), QString("1 - %1").arg(firstPath));
    QCOMPARE(actions[1]->text(), QString("2 - %1").arg(secondPath));

    Config reloaded = testConfig(application);
    QStringList const expectedRecentFiles{firstPath, secondPath};
    QCOMPARE(reloaded.recentFiles(), expectedRecentFiles);
}

void MainWindowTest::recentMenuAction_reopensFileAndPromotesEntry()
{
    QString const firstPath = createTestFile("reopen_first", "alpha");
    QString const secondPath = createTestFile("reopen_second", "beta");
    QVERIFY(!firstPath.isEmpty());
    QVERIFY(!secondPath.isEmpty());

    QString const application = uniqueName("recent_reopen");
    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();

    QVERIFY(window.openFileForTesting(firstPath));
    QVERIFY(window.openFileForTesting(secondPath));

    auto * menu = window.findChild<QMenu *>("menu_File");
    auto * viewer = window.findChild<Viewer *>("textBrowser");
    QVERIFY(menu != nullptr);
    QVERIFY(viewer != nullptr);

    QList<QAction *> actions = recentFileActions(*menu);
    QCOMPARE(actions.size(), 2);

    actions[1]->trigger();

    QCOMPARE(viewer->toPlainText(), QString("alpha"));
    QCOMPARE(
        window.windowTitle(),
        QString("Text Viewer - [%1]").arg(QFileInfo(firstPath).fileName()));

    actions = recentFileActions(*menu);
    QCOMPARE(actions[0]->text(), QString("1 - %1").arg(firstPath));
    QCOMPARE(actions[1]->text(), QString("2 - %1").arg(secondPath));

    Config reloaded = testConfig(application);
    QStringList const expectedRecentFiles{firstPath, secondPath};
    QCOMPARE(reloaded.recentFiles(), expectedRecentFiles);
}

void MainWindowTest::closeEvent_persistsWindowGeometry()
{
    QString const application = uniqueName("geometry");

    MainWindow window(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    window.init();
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    window.resize(640, 480);
    QByteArray const expected = window.saveGeometry();

    window.close();
    QVERIFY(window.isHidden());

    Config reloaded = testConfig(application);
    QCOMPARE(reloaded.windowGeometry(), expected);
}

QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"
