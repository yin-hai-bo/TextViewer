#include "config.h"
#include <QSettings>

static const char KEY_FONT[] = "font";
static const char KEY_FAMILY[] = "family";
static const char KEY_POINTSIZE[] = "pointSize";
static const char KEY_STYLE[] = "style";
static const char KEY_WEIGHT[] = "weight";

static const char KEY_WINDOW_STATE[] = "windowState";
static const char KEY_X[] = "x";
static const char KEY_Y[] = "y";
static const char KEY_WIDTH[] = "width";
static const char KEY_HEIGHT[] = "height";
static const char KEY_MAXIMIZED[] = "maximized";

static const char GROUP_VIEW[] = "view";
static const char KEY_LINE_HEIGHT[] = "lineHeight";

const char Config::KEY_RECENT_FILES[] = "recentFiles";

static const char KEY_LANGUAGE[] = "language";

static const QVariant EMPTY_STR(QString(""));

class FontConfig
{
public:
    static void save(QSettings & settings, const QFont & font)
    {
        SettingsGroupGuard sgg(settings, KEY_FONT);
        settings.setValue(KEY_FAMILY, font.family());
        settings.setValue(KEY_POINTSIZE, font.pointSize());
        settings.setValue(KEY_STYLE, font.style());
        settings.setValue(KEY_WEIGHT, font.weight());
    }

    static QFont load(QSettings & settings)
    {
        QFont result;
        SettingsGroupGuard sgg(settings, KEY_FONT);
        result.setFamily(settings.value(KEY_FAMILY, result.family()).toString());
        result.setPointSize(settings.value(KEY_POINTSIZE, result.pointSize()).toInt());
        result.setStyle((QFont::Style) settings.value(KEY_STYLE, (int) result.style()).toInt());
        result.setWeight((QFont::Weight) settings.value(KEY_WEIGHT, (int) result.weight()).toInt());
        return result;
    }
};

Config::Config()
    : settings_(QString("YHB"), QString("TextViewer"))
{}

void Config::setFont(const QFont & font)
{
    FontConfig::save(settings_, font);
}

QFont Config::font()
{
    return FontConfig::load(settings_);
}

void Config::setWindowState(const WindowState & state)
{
    SettingsGroupGuard sgg(settings_, KEY_WINDOW_STATE);
    settings_.setValue(KEY_MAXIMIZED, state.maximized);
    settings_.setValue(KEY_X, state.x);
    settings_.setValue(KEY_Y, state.y);
    settings_.setValue(KEY_WIDTH, state.width);
    settings_.setValue(KEY_HEIGHT, state.height);
}

bool Config::getWindowState(WindowState * state)
{
    SettingsGroupGuard sgg(settings_, KEY_WINDOW_STATE);
    if (settings_.childKeys().isEmpty()) {
        return false;
    }
    state->maximized = settings_.value(KEY_MAXIMIZED, state->maximized).toBool();
    state->x = settings_.value(KEY_X, state->x).toInt();
    state->y = settings_.value(KEY_Y, state->y).toInt();
    state->width = settings_.value(KEY_WIDTH, state->width).toInt();
    state->height = settings_.value(KEY_HEIGHT, state->height).toInt();
    return true;
}

QStringList Config::recentFiles()
{
    SettingsGroupGuard sgg(settings_, KEY_RECENT_FILES);

    auto keys = settings_.childKeys();
    if (keys.empty()) {
        return QStringList();
    }

    keys.sort();
    QStringList list;
    list.reserve(keys.size());

    for (const auto & key : keys) {
        list.push_back(settings_.value(key, EMPTY_STR).toString());
    }

    return list;
}

int Config::lineHeight()
{
    SettingsGroupGuard sgg(settings_, GROUP_VIEW);
    return settings_.value(KEY_LINE_HEIGHT, 100).toInt();
}

void Config::setLineHeight(int value)
{
    SettingsGroupGuard sgg(settings_, GROUP_VIEW);
    settings_.setValue(KEY_LINE_HEIGHT, value);
}

class LanguageName
{
private:
    struct Mapping
    {
        const Config::Language language;
        const char * name;
    };
    static const std::vector<Mapping> mapping_;

public:
    static const char * toString(Config::Language language)
    {
        auto const it
            = std::find_if(mapping_.cbegin(), mapping_.cend(), [language](const Mapping & m) {
                  return m.language == language;
              });
        return it == mapping_.cend() ? "" : it->name;
    }

    static Config::Language toLanguage(const QString & name)
    {
        auto const it = std::find_if(mapping_.cbegin(), mapping_.cend(), [name](const Mapping & m) {
            return name.compare(m.name, Qt::CaseInsensitive) == 0;
        });
        return it == mapping_.cend() ? Config::Language::System : it->language;
    }
};
const std::vector<LanguageName::Mapping> LanguageName::mapping_ = {
    {Config::Language::System, "System"},
    {Config::Language::English, "English"},
    {Config::Language::SimplifiedChinese, "Simplified Chinese"},
};

Config::Language Config::language()
{
    auto const value = settings_.value(KEY_LANGUAGE, EMPTY_STR).toString();
    return LanguageName::toLanguage(value);
}

void Config::setLanguage(Language language)
{
    auto const s = LanguageName::toString(language);
    settings_.setValue(KEY_LANGUAGE, s);
}
