#pragma once

#include <functional>

namespace __private_helpers_ValueTypePoly {

template <template<int a, int dummy> class T, int count>
struct LastNonEmpty {
    static const int last = !T<count, 0>::IS_EMPTY
    ? count: LastNonEmpty<T, count - 1>::last;
};

template <template<int a, int dummy> class T>
struct LastNonEmpty<T, -1> {
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
        *this = dummy;\
        return *this;\
    } \
    template<class T> \
    class_name& operator=(const std::shared_ptr<T>& pointer) {\
        Init<T, defined_interface_count_>::init(this, pointer.get()); \
        data_ = pointer; \
        return *this;\
    } \
    class_name& operator=(const class_name& a) {\
        Copier<defined_interface_count_, 0>::copy(this, &a);\
        return *this;\
    }\
private:\
    typedef class_name TheClass;\
    template <class T, int index> \
    struct Init { };\
    template <class T> /* seed */ \
    struct Init<T, -1> { \
        static void init(TheClass* inst, T* p) {} \
    }; \
    template < int index, int dummy> \
    struct Copier { \
        static const bool IS_EMPTY = true; \
    };\
    template <int dummy> \
    struct Copier<-1, dummy> { \
        static const bool IS_EMPTY = true; \
        static void copy(TheClass* theClass, const TheClass* another) { \
            theClass->data_ = another->data_;\
        } \
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
    static void init(TheClass* inst, T* p) { \
        Init<T, (index) - 1>::init(inst, p); \
        inst->init_##function_name(p); \
    } \
}; \
template <int dummy> \
struct Copier<index, dummy> { \
    static const bool IS_EMPTY = false; \
    static void copy(TheClass* theClass, const TheClass* another) {\
        Copier<(index) - 1, dummy>::copy(theClass, another);\
        const_cast<std::function<arguments>&>(theClass->function_name) \
            = another->function_name;\
    } \
};

#define INTERFACES_FOOTER \
protected: \
static const int defined_interface_count_ \
        = ::__private_helpers_ValueTypePoly::LastNonEmpty<Copier, interfaces_count_limit_ - 1>::last; \
static_assert(defined_interface_count_ > -1, "No interface defined!");\
std::shared_ptr<void> data_; \
};

