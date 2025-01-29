#include "lineheightdialog.h"
#include "ui_lineheightdialog.h"

static int validateValue(int value, const QSpinBox & spinBox)
{
    return std::max(spinBox.minimum(), std::min(spinBox.maximum(), value));
}

LineHeightDialog::LineHeightDialog(QWidget * parent, Config & config)
    : QDialog(parent)
    , ui(new Ui::LineHeightDialog)
{
    ui->setupUi(this);
    int value = validateValue(config.lineHeight(), *ui->spinBox);
    ui->spinBox->setValue(value);
}

int LineHeightDialog::lineHeight() const
{
    return validateValue(ui->spinBox->value(), *ui->spinBox);
}

LineHeightDialog::~LineHeightDialog() = default;
