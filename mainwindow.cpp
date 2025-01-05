#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFontDialog>
#include "utils.h"

constexpr qint64 MAX_FILE_LENGTH = 1024L * 1024 * 80;

static QString s_windowTitle;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    s_windowTitle = this->windowTitle();
    initStatusBar();
    initTextBrowser();
    on_actionClose_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        _labelTextLength->setText(tr("Text size: %1").arg(QLocale::system().toString(textSize)));
        return;
    } while (false);
    QMessageBox::warning(this, QString(), errorMsg);
}

void MainWindow::on_actionClose_triggered()
{
    ui->textBrowser->clear();
    this->setWindowTitle(s_windowTitle);
    ui->actionClose->setEnabled(false);
    _labelTextLength->setText(tr("(No document)"));
}

void MainWindow::initStatusBar()
{
    QStatusBar * const sb = ui->statusbar;
    assert(_labelTextLength == nullptr);
    _labelTextLength = new QLabel(sb);
    _labelTextLength->setMinimumWidth(160);
    _labelTextLength->setAlignment(Qt::AlignmentFlag::AlignCenter);
    sb->addWidget(_labelTextLength);
}

void MainWindow::initTextBrowser()
{
    ui->textBrowser->setFont(_config.font());
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
        _config.setFont(ui->textBrowser->font());
    } else {
        ui->textBrowser->setFont(oldFont);
    }
}
