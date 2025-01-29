#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QDialog>
#include <memory>

namespace Ui {
class AboutBox;
}

class AboutBox : public QDialog
{
    Q_OBJECT

public:
    explicit AboutBox(QWidget *parent = nullptr);
    ~AboutBox();

private:
    std::unique_ptr<Ui::AboutBox> ui;
};

#endif // ABOUTBOX_H
