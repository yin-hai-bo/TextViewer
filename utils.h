#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QVariant>

#include <cassert>
#include <memory>

template<typename T, size_t N> constexpr size_t array_size(T (&)[N])
{
    return N;
}

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

template<typename T>
class Connectable
{
    std::unique_ptr<T> t_;
    QMetaObject::Connection connection_;
    bool connected_ = false;

public:
    explicit Connectable(T * t = nullptr)
        : t_(t)
    {}
    Connectable(Connectable && other) = delete;
    Connectable & operator = (Connectable && other) = delete;
    ~Connectable() { disconnect(); }

    void reset(T * t = nullptr)
    {
        if (t != t_.get()) {
            disconnect();
            t_.reset(t);
        }
    }

    T * operator->() { return t_.operator->(); }
    T const * operator->() const { return t_.operator->(); }

    template<typename Func1, typename Func2>
    void connect(
        Func1 signal,
        const typename QtPrivate::ContextTypeForFunctor<Func2>::ContextType * context,
        Func2 && slot)
    {
        assert(!connected_);
        assert(t_);
        if (!connected_ && t_) {
            connection_ = QObject::connect(t_.get(), signal, context, std::forward<Func2>(slot));
            connected_ = true;
        }
    }

    void disconnect()
    {
        if (connected_) {
            QObject::disconnect(connection_);
            connected_ = false;
        }
    }

    operator bool() const { return t_.get() != nullptr; }
    bool operator!() const { return !operator bool(); }
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
