#include "config.h"
#include <QSettings>

static const char KEY_FONT[] = "font";
static const char KEY_FAMILY[] = "family";
static const char KEY_POINTSIZE[] = "pointSize";
static const char KEY_STYLE[] = "style";
static const char KEY_WEIGHT[] = "weight";

class FontConfig
{
public:
    static void save(QSettings & settings, const QFont & font)
    {
        settings.beginGroup(KEY_FONT);
        settings.setValue(KEY_FAMILY, font.family());
        settings.setValue(KEY_POINTSIZE, font.pointSize());
        settings.setValue(KEY_STYLE, font.style());
        settings.setValue(KEY_WEIGHT, font.weight());
        settings.endGroup();
    }

    static QFont load(QSettings & settings)
    {
        QFont result;
        settings.beginGroup(KEY_FONT);
        result.setFamily(settings.value(KEY_FAMILY, result.family()).toString());
        result.setPointSize(settings.value(KEY_POINTSIZE, result.pointSize()).toInt());
        result.setStyle((QFont::Style) settings.value(KEY_STYLE, (int) result.style()).toInt());
        result.setWeight((QFont::Weight) settings.value(KEY_WEIGHT, (int) result.weight()).toInt());
        settings.endGroup();
        return result;
    }

};

Config::Config()
    : _settings(QString("YHB"), QString("TextViewer"))
{}

void Config::setFont(const QFont & font)
{
    FontConfig::save(_settings, font);
}

QFont Config::font()
{
    return FontConfig::load(_settings);
}
