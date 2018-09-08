#pragma once

#include <functional>

namespace __private_helpers_ValueTypePoly {

template <template<class T, int a> class T, class ActualT, int count>
struct LastNonEmpty {
    static const int last = !T<ActualT, count>::IS_EMPTY
    ? count: LastNonEmpty<T, ActualT, count - 1>::last;
};

template <template<class T, int a> class T, class ActualT>
struct LastNonEmpty<T, ActualT, -1> {
    static const int last = -1;
};

template <class C, typename Ret, typename ... Ts>
std::function<Ret(Ts...)> bind_this(C* c, Ret (C::*m)(Ts...))
{
    return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

template <class C, typename Ret, typename ... Ts>
std::function<Ret(Ts...)> bind_this(const C* c, Ret (C::*m)(Ts...) const)
{
    return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}
    
}

#define DEFAULT_INTERFACES_COUNT_LIMIT 10

#define INTERFACES_HEADER(class_name) INTERFACES_HEADER_EX(class_name, DEFAULT_INTERFACES_COUNT_LIMIT)

#define INTERFACES_HEADER_EX(class_name, interfaces_count_limit) \
class class_name { \
    static const int interfaces_count_limit_ = interfaces_count_limit;\
public:\
    class_name(const class_name& another) = default;\
    template<class T>\
    class_name(const T& a) {*this = a;}\
    class_name() {}\
    template<class T> \
    class_name& operator=(const T& a) { \
        std::shared_ptr<T> dummy = std::make_shared<T>(a); \
        static const int last = ::__private_helpers_ValueTypePoly::LastNonEmpty<Init, T, interfaces_count_limit - 1>::last; \
        static_assert(last > -1, "No interface defined!");\
        Init<T, last>::init(this, dummy.get()); \
        data_ = std::move(dummy); \
        return *this;\
    } \
private:\
    typedef class_name TheClass;\
    template <class T, int index> \
    struct Init { \
        static const bool IS_EMPTY = true; \
    }; \
    template <class T> /* seed */ \
    struct Init<T, -1> { \
        static const bool IS_EMPTY = true; \
        static void init(TheClass* inst, T* p) {} \
    }; \
    template <class T> \
    struct Init<T, interfaces_count_limit> { \
        static const bool IS_EMPTY = true; \
    };


#define INTERFACE(index, function_name, arguments) \
public:\
static_assert((index) < interfaces_count_limit_ && (index) >= 0, \
            "Interface index out of range!");\
const std::function<arguments> function_name; \
private:\
template<class T> \
void init_##function_name(T *p) { \
    const_cast<std::function<arguments>&>(function_name) \
            = ::__private_helpers_ValueTypePoly::bind_this(p, &T::function_name); \
} \
template <class T> \
struct Init<T, index> { \
    static const bool IS_EMPTY = false; \
    static void init(TheClass* inst, T* p) { \
        Init<T, (index) - 1>::init(inst, p); \
        inst->init_##function_name(p); \
    } \
};

#define INTERFACES_FOOTER \
protected: \
std::shared_ptr<void> data_; \
};

