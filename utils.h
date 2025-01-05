#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QVariant>

class ConnectGuard
{
    QMetaObject::Connection _connection;

public:
    template<typename Func1, typename Func2>
    ConnectGuard(
        const typename QtPrivate::FunctionPointer<Func1>::Object * sender,
        Func1 signal,
        const typename QtPrivate::ContextTypeForFunctor<Func2>::ContextType * context,
        Func2 && slot)
        : _connection(QObject::connect(sender, signal, context, std::forward<Func2>(slot)))
    {}

    template<typename Func1, typename Func2>
    ConnectGuard(
        const typename QtPrivate::FunctionPointer<Func1>::Object * sender,
        Func1 signal,
        Func2 && slot)
        : _connection(QObject::connect(sender, signal, std::forward<Func2>(slot)))
    {}

    ConnectGuard(const ConnectGuard &) = delete;
    ConnectGuard & operator=(const ConnectGuard &) = delete;

    ConnectGuard(ConnectGuard && other)
        : _connection(std::move(other._connection))
    {}

    ~ConnectGuard() { QObject::disconnect(_connection); }
};

struct Utils
{
    template<typename Value, typename Key>
    static bool tryGetMapValue(const QMap<Key, QVariant> & map, const Key & key, Value * result)
    {
        auto const it = map.find(key);
        if (it != map.cend() && it->template canConvert<Value>()) {
            *result = it->template value<Value>();
            return true;
        }
        return false;
    }

    template<typename Value, typename Key>
    static typename std::remove_reference<Value>::type getMapValueOrDefault(
        const QMap<Key, QVariant> & map, const Key & key, Value && defaultValue)
    {
        typedef typename std::remove_reference<Value>::type result_type;
        return getMapValueOrDefaultGen<Value, Key>(map, key, [&]() -> result_type { return defaultValue; });
    }

    template<typename Value, typename Key>
    static typename std::remove_reference<Value>::type getMapValueOrDefaultGen(
        const QMap<Key, QVariant> & map, const Key & key, std::function<Value(void)> defaultValueGenerator)
    {
        auto const it = map.find(key);
        if (it != map.cend() && it->template canConvert<Value>()) {
            return it->template value<Value>();
        }
        return defaultValueGenerator();
    }
};

#endif // UTILS_H
