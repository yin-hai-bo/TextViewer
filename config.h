#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QFont>

class Config
{
public:
    Config();

    void setFont(const QFont & font);
    QFont font();

private:
    QSettings _settings;

};

#endif // CONFIG_H
