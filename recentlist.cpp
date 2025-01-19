#include "recentlist.h"

using namespace std;

typedef std::unique_ptr<QAction> ActionPtr;

static QString makeActionName(const QString & filename, int index)
{
    return QString("%1 - %2").arg(index + 1).arg(filename);
}

RecentList::RecentList(size_t maxFiles, IRecentFileSignalReceiver & receiver, QMenu & menu)
    : maxFiles_(maxFiles)
    , recentFileSignalReceiver_(receiver)
    , menu_(menu)
{
    QObject::connect(&signalMapper_, &QSignalMapper::mappedInt, this, &RecentList::onActionTriggered);
}

RecentList::~RecentList()
{
    this->filenames_.clear();
    for (auto & action : this->actions_) {
        QObject::disconnect(
            action.get(),
            &QAction::triggered,
            &this->signalMapper_,
            qOverload<>(&QSignalMapper::map));
    }
    this->actions_.clear();
}

void RecentList::init(QStringList && filenames)
{
    assert(this->filenames_.empty());
    assert(this->actions_.empty());

    QStringList::iterator it = filenames.begin();
    while (this->filenames_.size() < maxFiles_ && it != filenames.end()) {
        this->filenames_.push_back(std::move(*it++));
    }

    if (!this->filenames_.empty()) {
        this->menu_.addSeparator();
    }

    for (const QString & filename : this->filenames_) {
        ActionPtr action(new QAction);
        this->menu_.addAction(action.get());
        this->actions_.push_back(std::move(action));
    }

    resetActions();
}

void RecentList::insertToFront(const QString & filename)
{
    auto const it = find(this->filenames_.begin(), filenames_.end(), filename);
    if (it != this->filenames_.end()) {
        QString s(std::move(*it));
        this->filenames_.erase(it);
        this->filenames_.push_front(std::move(s));
    } else {
        filenames_.push_front(filename);
        if (filenames_.size() > maxFiles_) {
            filenames_.pop_back();
        } else {
            ActionPtr action(new QAction);
            if (!this->filenames_.empty()) {
                this->menu_.addSeparator();
            }
            this->menu_.addAction(action.get());
            this->actions_.push_back(std::move(action));
        }
    }
    resetActions();
}

void RecentList::erase(int index)
{
    this->filenames_.erase(this->filenames_.begin() + index);
    auto & action = this->actions_.back();
    QObject::disconnect(
        action.get(), &QAction::triggered, &this->signalMapper_, qOverload<>(&QSignalMapper::map));
    this->actions_.pop_back();
    this->resetActions();
}

void RecentList::onActionTriggered(int index) const
{
    recentFileSignalReceiver_.onActionRecentFile(index, filenames_[index]);
}

void RecentList::resetActions()
{
    assert(this->actions_.size() == this->filenames_.size());

    int index = 0;
    for (auto & action : this->actions_) {
        action->setText(makeActionName(filenames_[index], index));
        QObject::connect(
            action.get(),
            &QAction::triggered,
            &signalMapper_,
            qOverload<>(&QSignalMapper::map),
            Qt::UniqueConnection);
        signalMapper_.setMapping(action.get(), index);
        ++index;
    }
}
