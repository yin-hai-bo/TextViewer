#ifndef LINEHEIGHTDIALOG_H
#define LINEHEIGHTDIALOG_H

#include <QDialog>
#include <memory>
#include "config.h"

namespace Ui {
class LineHeightDialog;
}

class LineHeightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LineHeightDialog(QWidget * parent, Config & config);
    ~LineHeightDialog();

    int lineHeight() const;

private:
    std::unique_ptr<Ui::LineHeightDialog> ui;
};

#endif // LINEHEIGHTDIALOG_H
