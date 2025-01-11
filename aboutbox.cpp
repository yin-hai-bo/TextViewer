#include "aboutbox.h"
#include "ui_aboutbox.h"

AboutBox::AboutBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBox)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon());
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);
}

AboutBox::~AboutBox()
{
    delete ui;
}
