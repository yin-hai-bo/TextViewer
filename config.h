#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QFont>

class Config
{
public:
    struct WindowState
    {
        bool maximized;
        int x;
        int y;
        int width;
        int height;
    };

public:
    Config();

    void setFont(const QFont & font);
    QFont font();

    void setWindowState(const WindowState &);
    bool getWindowState(WindowState *);

private:
    QSettings settings_;

};

#endif // CONFIG_H
