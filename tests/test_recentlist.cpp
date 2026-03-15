#include <QtTest>

#include "recentlist.h"

class FakeRecentFileReceiver : public RecentList::IRecentFileSignalReceiver
{
public:
    int index = -1;
    QString filename;

    void onActionRecentFile(int newIndex, const QString & newFilename) override
    {
        index = newIndex;
        filename = newFilename;
    }
};

class RecentListTest : public QObject
{
    Q_OBJECT

private slots:
    void insertToFront_reordersExistingEntries();
    void triggeringAction_reportsMappedEntry();

private:
    static QList<QAction *> recentActions(const QMenu & menu);
};

QList<QAction *> RecentListTest::recentActions(const QMenu & menu)
{
    QList<QAction *> result;
    for (QAction * action : menu.actions()) {
        if (!action->isSeparator()) {
            result.push_back(action);
        }
    }
    return result;
}

void RecentListTest::insertToFront_reordersExistingEntries()
{
    QMenu menu;
    FakeRecentFileReceiver receiver;
    RecentList list(12, receiver, menu);

    list.init(QStringList{"a.txt", "b.txt", "c.txt"});
    list.insertToFront("b.txt");

    QStringList actual;
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        actual.push_back(*it);
    }

    QCOMPARE(actual, QStringList({"b.txt", "a.txt", "c.txt"}));
}

void RecentListTest::triggeringAction_reportsMappedEntry()
{
    QMenu menu;
    FakeRecentFileReceiver receiver;
    RecentList list(12, receiver, menu);

    list.init(QStringList{"alpha.txt", "beta.txt", "gamma.txt"});

    QList<QAction *> actions = recentActions(menu);
    QCOMPARE(actions.size(), 3);

    actions.at(1)->trigger();

    QCOMPARE(receiver.index, 1);
    QCOMPARE(receiver.filename, QString("beta.txt"));
}

QTEST_MAIN(RecentListTest)
#include "test_recentlist.moc"
