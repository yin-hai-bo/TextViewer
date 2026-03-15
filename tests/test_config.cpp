#include <QtTest>
#include <QTemporaryDir>
#include <QUuid>

#include "config.h"

namespace
{
QTemporaryDir & settingsRoot()
{
    static QTemporaryDir DIR;
    return DIR;
}

void configureTestSettingsBackend()
{
    static bool CONFIGURED = false;
    if (CONFIGURED) {
        return;
    }

    QVERIFY2(settingsRoot().isValid(), "Failed to create temporary settings directory");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsRoot().path());
    CONFIGURED = true;
}
}

class ConfigTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void recentFiles_preserveOrderBeyondTen();
    void lineHeight_defaultsAndPersists();
    void windowGeometry_roundTrips();

private:
    static QString uniqueName(const char * suffix);
    static void clearSettings(const QString & application);
};

QString ConfigTest::uniqueName(const char * suffix)
{
    return QString("TextViewerTests_%1_%2").arg(suffix).arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

static Config testConfig(const QString & application)
{
    configureTestSettingsBackend();
    return Config(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
}

void ConfigTest::clearSettings(const QString & application)
{
    configureTestSettingsBackend();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "YHB", application);
    settings.clear();
    settings.sync();
}

void ConfigTest::initTestCase()
{
    configureTestSettingsBackend();
}

void ConfigTest::recentFiles_preserveOrderBeyondTen()
{
    QString const application = uniqueName("recent_files");
    clearSettings(application);

    Config config = testConfig(application);
    QStringList expected;
    for (int i = 1; i <= 12; ++i) {
        expected.push_back(QString("file-%1.txt").arg(i));
    }

    config.setRecentFiles(expected.cbegin(), expected.cend());

    QCOMPARE(config.recentFiles(), expected);
    clearSettings(application);
}

void ConfigTest::lineHeight_defaultsAndPersists()
{
    QString const application = uniqueName("line_height");
    clearSettings(application);

    Config config = testConfig(application);
    QCOMPARE(config.lineHeight(), 100);

    config.setLineHeight(135);

    Config reloaded = testConfig(application);
    QCOMPARE(reloaded.lineHeight(), 135);
    clearSettings(application);
}

void ConfigTest::windowGeometry_roundTrips()
{
    QString const application = uniqueName("window_geometry");
    clearSettings(application);

    Config config = testConfig(application);
    QByteArray const expected = QByteArray::fromHex("010203A0FF");

    config.setWindowGeometry(expected);

    Config reloaded = testConfig(application);
    QCOMPARE(reloaded.windowGeometry(), expected);
    clearSettings(application);
}

QTEST_MAIN(ConfigTest)
#include "test_config.moc"
