#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QFont>
#include <QList>
#include <QStringView>

class SettingsGroupGuard
{
    QSettings & settings_;
public:
    SettingsGroupGuard(QSettings & settings, const char * prefix)
        : settings_(settings)
    {
        settings_.beginGroup(prefix);
    }
    SettingsGroupGuard(const SettingsGroupGuard &);
    SettingsGroupGuard & operator=(const SettingsGroupGuard &);
    ~SettingsGroupGuard() { settings_.endGroup(); }
};

class Config
{
    static const char KEY_RECENT_FILES[];
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

    template<typename It>
    void setRecentFiles(It first, It last)
    {
        SettingsGroupGuard sgg(settings_, KEY_RECENT_FILES);

        auto keys = settings_.childKeys();
        for (const auto & s : keys) {
            settings_.remove(s);
        }

        int i = 1;
        while (first != last) {
            char key[64];
            _ltoa_s(i, key, 10);
            settings_.setValue(key, *first);
            ++first;
            ++i;
        }
    }

    QStringList recentFiles();

private:
    QSettings settings_;


};

#endif // CONFIG_H
