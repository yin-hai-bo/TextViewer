#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QFontDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QScreen>
#include "utils.h"
#include "aboutbox.h"
#include "lineheightdialog.h"

constexpr qint64 MAX_FILE_LENGTH = 1024L * 1024 * 80;
constexpr int MAX_RECENT_FILES = 5;

static QString s_windowTitle;

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
    recentList_.reset(new RecentList(
        MAX_RECENT_FILES,
        *this,
        *ui->menu_File));
    s_windowTitle = this->windowTitle();
}

MainWindow::~MainWindow() = default;

void MainWindow::init(const QApplication & app)
{
    initWindowState(app);
    initStatusBar();
    initTextBrowser();
    initRecentFilesMenu();
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

bool MainWindow::eventFilter(
    QObject *obj,
    QEvent *event)
{
    if (!documentOpened_ || obj != ui->textBrowser) {
        return QObject::eventFilter(obj, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::MiddleButton) {
            qDebug() << "Middle button pressed";
        }
    }

    return QObject::eventFilter(obj, event);
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
            QString("%1 [%2]").arg(s_windowTitle).arg(QFileInfo(filename).fileName()));
        ui->actionClose->setEnabled(true);

        int const textSize = ui->textBrowser->document()->characterCount();
        statusBarLabel_TotalLength_->setText(tr("Text size: %1").arg(QLocale::system().toString(textSize)));

        this->addFileToRecents(filename);

        setTextBrowserLineHeight(ui->textBrowser, config_.lineHeight());

        return documentOpened_ = true;
    } while (false);

    QMessageBox::warning(this, QString(), errorMsg);
    return false;
}

void MainWindow::on_actionClose_triggered()
{
    ui->textBrowser->clear();
    this->setWindowTitle(s_windowTitle);
    ui->actionClose->setEnabled(false);
    statusBarLabel_TotalLength_->setText(tr("(No document)"));
    documentOpened_ = false;
}

void MainWindow::initWindowState(const QApplication & app)
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
    QStatusBar * const sb = ui->statusbar;
    assert(statusBarLabel_TotalLength_ == nullptr);
    statusBarLabel_TotalLength_ = new QLabel(sb);
    statusBarLabel_TotalLength_->setMinimumWidth(160);
    statusBarLabel_TotalLength_->setAlignment(Qt::AlignmentFlag::AlignCenter);
    sb->addWidget(statusBarLabel_TotalLength_);
}

void MainWindow::initTextBrowser()
{
    QTextBrowser & tb = *ui->textBrowser;
    tb.setFont(config_.font());
    tb.installEventFilter(this);
}

void MainWindow::initRecentFilesMenu()
{
    this->recentList_->init(config_.recentFiles());
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

void MainWindow::onActionRecentFile(int index, const QString & filename)
{
    qDebug() << "Recent File #" << index << ": " << filename;
    if (!this->openFile(filename)) {
        this->recentList_->erase(index);
        this->config_.setRecentFiles(recentList_->cbegin(), recentList_->cend());
    }
}
