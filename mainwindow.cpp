#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QActionGroup>
#include <QFileDialog>
#include <QFile>
#include <QFontDialog>
#include <QMessageBox>
#include <QScreen>
#include "aboutbox.h"
#include "lineheightdialog.h"
#include "utils.h"
#include "viewer.h"

constexpr qint64 MAX_FILE_LENGTH = 1024L * 1024 * 80;
constexpr int MAX_RECENT_FILES = 5;

static QString s_windowTitle;

enum StatusBarLabel {
    DOCUMENT_LENGTH,
    AUTO_SCROLLING,
    CLICK_TO_AUTO_SCROLLING,
};

static void setTextBrowserLineHeight(QTextBrowser * tb, int value)
{
    QTextBlockFormat blockFormat;
    blockFormat.setLineHeight(value, QTextBlockFormat::ProportionalHeight);

    QTextCursor cursor(tb->document());
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(blockFormat);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    autoScrollState_.enabled = false;
    recentList_.reset(new RecentList(
        MAX_RECENT_FILES,
        *this,
        *ui->menu_File));
    s_windowTitle = this->windowTitle();
}

MainWindow::~MainWindow() = default;

void MainWindow::init()
{
    initWindowState();
    initStatusBar();
    initTextBrowser();
    initRecentFilesMenu();
    initLanguageMenu();
    setLanguage(config_.language());
    on_actionClose_triggered();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    Config::WindowState state;
    state.maximized = this->isMaximized();
        state.x = this->x();
        state.y = this->y();
        state.width = this->width();
        state.height = this->height();
    config_.setWindowState(state);
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Open a text file to view"),
        QString(),
        tr("Text files (*.txt);;All files (*)"),
        nullptr);
    if (filename.isEmpty()) {
        return;
    }

    openFile(filename);
}

bool MainWindow::openFile(const QString & filename) {
    QString errorMsg;
    do {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            errorMsg = tr("Open file failed.");
            break;
        }

        auto const fileLength = file.size();
        if (fileLength == 0) {
            errorMsg = tr("It's an empty file.");
            break;
        }

        if (fileLength > MAX_FILE_LENGTH) {
            errorMsg = tr("Too large file.");
            break;
        }

        QTextStream in(&file);
        ui->textBrowser->setPlainText(in.readAll());

        this->setWindowTitle(
            QString("%1 - [%2]").arg(s_windowTitle).arg(QFileInfo(filename).fileName()));
        ui->actionClose->setEnabled(true);

        int const textSize = ui->textBrowser->document()->characterCount();
        statusBarLabels_[DOCUMENT_LENGTH]->setText(tr("Text size: %1").arg(QLocale::system().toString(textSize)));
        statusBarLabels_[CLICK_TO_AUTO_SCROLLING]->setText(tr("Click mouse middle button to on/off auto scrolling ..."));

        this->addFileToRecents(filename);

        setTextBrowserLineHeight(ui->textBrowser, config_.lineHeight());

        ui->textBrowser->documentOpened();
        return true;
    } while (false);

    QMessageBox::warning(this, QString(), errorMsg);
    return false;
}

void MainWindow::on_actionClose_triggered()
{
    ui->textBrowser->clear();
    this->setWindowTitle(s_windowTitle);
    ui->actionClose->setEnabled(false);
    statusBarLabels_[DOCUMENT_LENGTH]->setText(tr("(No document)"));
    statusBarLabels_[CLICK_TO_AUTO_SCROLLING]->setText("");
    ui->textBrowser->documentClosed();
}

void MainWindow::initWindowState()
{
    QRect geometry = QGuiApplication::primaryScreen()->geometry();

    Config::WindowState state;
    state.maximized = false;
    state.width = geometry.width() / 2;
    state.height = geometry.height() / 2;
    state.x = state.width / 2;
    state.y = state.height / 2;

    config_.getWindowState(&state);

    this->resize(std::max(80, state.width), std::max(80, state.height));
    if (state.maximized) {
        this->setWindowState(Qt::WindowMaximized);
    }
}

void MainWindow::initStatusBar()
{
    assert(statusBarLabels_[0] == nullptr);

    QStatusBar * const sb = ui->statusbar;
    for (size_t i = 0; i < statusBarLabels_.size(); ++i) {
        auto label = statusBarLabels_[i] = new QLabel(sb);
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sb->addWidget(label);
    }
}

static const QString & decideAutoScrollStateDescription(bool onOff, Viewer::Direction dir)
{
    static const QString EMPTY("");
    static const QString NONE = QObject::tr("Auto Scrolling");
    static const QString UP = QObject::tr("Auto Scrolling (UP)");
    static const QString DOWN = QObject::tr("Auto Scrolling (DOWN)");
    if (!onOff) {
        return EMPTY;
    }
    switch (dir) {
    case Viewer::Direction::Up:
        return UP;
    case Viewer::Direction::Down:
        return DOWN;
    default:
        return NONE;
    }
}

void MainWindow::initTextBrowser()
{
    Viewer * const v = ui->textBrowser;
    v->setFont(config_.font());
    connect(v, &Viewer::autoScrollStateChangeSignal, [this](bool onOff, Viewer::Direction dir) {
        this->statusBarLabels_[AUTO_SCROLLING]->setText(
            decideAutoScrollStateDescription(onOff, dir));
    });
}

void MainWindow::initRecentFilesMenu()
{
    this->recentList_->init(config_.recentFiles());
}

void MainWindow::resetLanguage()
{
    if (translator_) {
        QApplication::removeTranslator(translator_.get());
        translator_.reset();
    }
}

void MainWindow::changeLanguage(const QStringList & uiLanguages)
{
    std::unique_ptr<QTranslator> t = std::make_unique<QTranslator>();
    for (const QString & locale : uiLanguages) {
        const QString resName = ":/i18n/TextViewer_" + QLocale(locale).name();
        if (t->load(resName)) {
            resetLanguage();
            translator_ = std::move(t);
            QApplication::installTranslator(translator_.get());
            return;
        }
    }
}

void MainWindow::setLanguage(Config::Language language)
{
    switch (language) {
    case Config::Language::English:
        resetLanguage();
        break;
    case Config::Language::SimplifiedChinese:
        changeLanguage(QStringList{QString("zh-CN")});
        break;
    default:
        changeLanguage(QLocale::system().uiLanguages());
        break;
    }
}

void MainWindow::initLanguageMenu()
{
    std::array actions{
        ui->actionLanguageSystem, ui->actionLanguageEnglish, ui->actionLanguageSimplifiedChinese};

    QActionGroup * group = new QActionGroup(this);
    for (QAction * a : actions) {
        group->addAction(a);
        connect(a, &QAction::triggered, this, &MainWindow::onActionLanguageTriggered);
    }

    QAction * action;
    switch (config_.language()) {
    case Config::Language::English:
        action = ui->actionLanguageEnglish;
        break;
    case Config::Language::SimplifiedChinese:
        action = ui->actionLanguageSimplifiedChinese;
        break;
    default:
        action = ui->actionLanguageSystem;
        break;
    }
    action->setChecked(true);
}

void MainWindow::addFileToRecents(const QString & filename)
{
    recentList_->insertToFront(filename);
    config_.setRecentFiles(recentList_->cbegin(), recentList_->cend());
}

void MainWindow::on_actionFont_triggered()
{
    QFont oldFont = ui->textBrowser->font();

    QFontDialog dialog(this);
    dialog.setCurrentFont(oldFont);
    ConnectGuard cg(&dialog, &QFontDialog::currentFontChanged, [=](const QFont & font) {
        ui->textBrowser->setFont(font);
    });
    if (QDialog::DialogCode::Accepted == dialog.exec()) {
        config_.setFont(ui->textBrowser->font());
    } else {
        ui->textBrowser->setFont(oldFont);
    }
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui->textBrowser->zoomIn();
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->textBrowser->zoomOut();
}

void MainWindow::on_actionRestoreDefaultZoom_triggered()
{
    ui->textBrowser->setFont(config_.font());
}

void MainWindow::on_actionAbout_triggered()
{
    AboutBox box(this);
    box.exec();
}

void MainWindow::on_actionLineHeight_triggered()
{
    LineHeightDialog dlg(this, this->config_);
    if (QDialog::Accepted == dlg.exec()) {
        int value = dlg.lineHeight();
        this->config_.setLineHeight(value);
        setTextBrowserLineHeight(ui->textBrowser, value);
    }
}

void MainWindow::onActionLanguageTriggered()
{
    QAction * const action = qobject_cast<QAction *>(sender());
    Config::Language newLanguage;
    if (action == ui->actionLanguageEnglish) {
        newLanguage = Config::Language::English;
    } else if (action == ui->actionLanguageSimplifiedChinese) {
        newLanguage = Config::Language::SimplifiedChinese;
    } else {
        newLanguage = Config::Language::System;
    }
    if (config_.language() != newLanguage) {
        config_.setLanguage(newLanguage);
        setLanguage(newLanguage);
    }
}

void MainWindow::onActionRecentFile(int index, const QString & filename)
{
    if (!this->openFile(filename)) {
        this->recentList_->erase(index);
        this->config_.setRecentFiles(recentList_->cbegin(), recentList_->cend());
    }
}

void MainWindow::changeEvent(QEvent * event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}
