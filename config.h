#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QFont>
#include <QList>
#include <QChar>
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
    enum class Language {
        System,
        English,
        SimplifiedChinese,
    };

public:
    Config();

    void setFont(const QFont & font);
    QFont font();

    void setWindowGeometry(const QByteArray & geometry);
    QByteArray windowGeometry();

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
            constexpr int RECENT_FILE_KEY_WIDTH = 3;
            settings_.setValue(
                QString("%1").arg(i, RECENT_FILE_KEY_WIDTH, 10, QChar('0')),
                *first);
            ++first;
            ++i;
        }
    }

    QStringList recentFiles();

    int lineHeight();
    void setLineHeight(int value);

    Language language();
    void setLanguage(Language);

private:
    QSettings settings_;


};

#endif // CONFIG_H
