#include <QtTest>
#include <QSignalSpy>

#include "viewer.h"

Q_DECLARE_METATYPE(Viewer::Direction)

class ViewerTest : public QObject
{
    Q_OBJECT

private slots:
    void middleClick_togglesAutoScroll();
};

void ViewerTest::middleClick_togglesAutoScroll()
{
    qRegisterMetaType<Viewer::Direction>();

    Viewer viewer;
    viewer.resize(320, 240);
    viewer.setPlainText(QString(400, 'x'));
    viewer.documentOpened();
    viewer.show();
    QVERIFY(QTest::qWaitForWindowExposed(&viewer));

    QSignalSpy spy(&viewer, &Viewer::autoScrollStateChangeSignal);
    QVERIFY(spy.isValid());

    QPoint const center = viewer.viewport()->rect().center();

    QTest::mousePress(viewer.viewport(), Qt::MiddleButton, Qt::NoModifier, center);
    QTest::mouseRelease(viewer.viewport(), Qt::MiddleButton, Qt::NoModifier, center);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(qvariant_cast<Viewer::Direction>(spy.at(0).at(1)), Viewer::Direction::None);
    QVERIFY(viewer.isAutoScrollState());

    QTest::mousePress(viewer.viewport(), Qt::MiddleButton, Qt::NoModifier, center);
    QTest::mouseRelease(viewer.viewport(), Qt::MiddleButton, Qt::NoModifier, center);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
    QCOMPARE(qvariant_cast<Viewer::Direction>(spy.at(1).at(1)), Viewer::Direction::None);
    QVERIFY(!viewer.isAutoScrollState());
}

QTEST_MAIN(ViewerTest)
#include "test_viewer.moc"
