#ifndef RECENTLIST_H
#define RECENTLIST_H

#include <deque>
#include <memory>

#include <QAction>
#include <QMenu>
#include <QSignalMapper>

class RecentList : public QObject
{
public:
    struct IRecentFileSignalReceiver
    {
        virtual void onActionRecentFile(int index, const QString & filename) = 0;
    };

    typedef std::deque<QString>::const_iterator filename_it;

    RecentList(size_t maxFiles, IRecentFileSignalReceiver & receiver, QMenu & menu);
    RecentList(const RecentList &) = delete;
    RecentList & operator=(const RecentList &) = delete;
    ~RecentList();

    void init(QStringList && filename);
    void insertToFront(const QString & filename);
    void erase(int index);

    filename_it cbegin() const { return filenames_.cbegin(); }
    filename_it cend() const { return filenames_.cend(); }

private slots:
    void onActionTriggered(int index) const;

private:
    void resetActions();

    const size_t maxFiles_;
    IRecentFileSignalReceiver & recentFileSignalReceiver_;
    QMenu & menu_;

    QSignalMapper signalMapper_;
    std::deque<QString> filenames_;
    std::deque<std::unique_ptr<QAction> > actions_;
};

#endif // RECENTLIST_H
