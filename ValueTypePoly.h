//
//  隹王六十又八年既死霸乙已
//  Created by AmateurCat on 09/08/2018.
//  Copyright © 2017 AmateurCat. No rights reserved.
//
#pragma once

#include <functional>

#define INTERFACE_SIGNATURE INTERFACE_3141592653345678

namespace __private_helpers {


template <template<int a, int dummy> class T, int count>
struct LastNonEmpty {
    static const int last = !T<count, 0>::IS_EMPTY
    ? count: LastNonEmpty<T, count - 1>::last;
};

template <template<int a, int dummy> class T>
struct LastNonEmpty<T, -1> {
    static const int last = -1;
};

template <typename T, typename Another = int>
struct IsInterface : std::false_type { };

template <typename T>
struct IsInterface <T, decltype(&T::INTERFACE_SIGNATURE, 0)> : std::true_type { };
}

namespace __private_delegate {

template<typename Ret, typename ... Ts>
class BaseCallable {
public:
    virtual Ret call(Ts&&... args) = 0;
};

template<class T, typename Ret, typename ... Ts>
class CallableConst : public BaseCallable<Ret, Ts...> {
public:
    typedef Ret (T::*ConstMemFunction)(Ts...) const;
    CallableConst(T* inst, ConstMemFunction memfunction) : inst_(inst), func_(memfunction) {
    }
    virtual Ret call(Ts&&... args) {
        return (inst_->*func_)(std::forward<decltype(args)>(args)...);
    }
private:
    T* inst_;
    ConstMemFunction func_;
};

template<class T, typename Ret, typename ... Ts>
class Callable : public BaseCallable<Ret, Ts...> {
public:
    typedef Ret (T::*MemFunction)(Ts...);
    Callable(T* inst, MemFunction memfunction) : inst_(inst), func_(memfunction) {
    }
    virtual Ret call(Ts&&... args) {
        return (inst_->*func_)(std::forward<decltype(args)>(args)...);
    }
private:
    T* inst_;
    MemFunction func_;
};

template <typename F>
class MemFunction;

template <typename R, typename... Args>
class MemFunction<R(Args...)> {
public:
    MemFunction() {}
    template<class T, typename Ret, typename ... Ts>
    static MemFunction<Ret(Ts...)> from(T* object, Ret (T::*mem_function)(Ts...) const) {
        return MemFunction(object, mem_function);
    }
    template<class T, typename Ret, typename ... Ts>
    static MemFunction<Ret(Ts...)> from(T* object, Ret (T::*mem_function)(Ts...)) {
        return MemFunction(object, mem_function);
    }
    template<class T, typename Ret, typename ... Ts>
    MemFunction(T* object, Ret (T::*mem_function)(Ts...) const)
    : callable_(std::make_shared<CallableConst<T, Ret, Ts...>>(object, mem_function)) {
    }
    template<class T, typename Ret, typename ... Ts>
    MemFunction(T* object, Ret (T::*mem_function)(Ts...))
    : callable_(std::make_shared<Callable<T, Ret, Ts...>>(object, mem_function)) {
    }
    R operator()(Args&&... args) const {
        return callable_->call(std::forward<decltype(args)>(args)...);
    }
private:
    std::shared_ptr<BaseCallable<R, Args...>> callable_;
};

}

#define DEFAULT_INTERFACES_COUNT_LIMIT 10

#define INTERFACES_HEADER(class_name) INTERFACES_HEADER_EX(class_name, DEFAULT_INTERFACES_COUNT_LIMIT)

#define INTERFACES_HEADER_EX(class_name, interfaces_count_limit) \
class class_name { \
    static const int interfaces_count_limit_ = interfaces_count_limit;\
    template<class arguments>\
    using MemFunctionType = ::__private_delegate::MemFunction<arguments>;\
    template<class Arg1, class Arg2 = int>\
    using IsInterface = ::__private_helpers::IsInterface<Arg1, Arg2>;\
public:\
    static void INTERFACE_SIGNATURE() {}\
    class_name(const class_name& another) = default;\
    template<class T>\
    class_name(const T& a) {*this = a;}\
    class_name() {}\
    template<class T, \
    typename std::enable_if<std::is_copy_constructible<T>::value && !IsInterface<T>::value>::type* value = nullptr> \
    class_name& operator=(const T& a) { \
        std::shared_ptr<T> dummy = std::make_shared<T>(a); \
        *this = dummy;\
        return *this;\
    } \
    template<class T, \
    typename std::enable_if<std::is_copy_constructible<T>::value && !IsInterface<T>::value>::type* value = nullptr> \
    class_name& operator=(const std::shared_ptr<T>& pointer) {\
        Init<T, defined_interface_count_>::init(this, pointer.get()); \
        data_ = pointer; \
        return *this;\
    } \
    template<class T, \
    typename std::enable_if<IsInterface<T>::value>::type* value = nullptr> \
    class_name& operator=(const T& pointer) {\
        Copier<defined_interface_count_, 0>::copy(this, &pointer);\
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
        template<class AnotherType>\
        static void copy(TheClass* theClass, const AnotherType* another) { \
            theClass->data_ = another->getDataPointer();\
        } \
    };


#define INTERFACE(index, function_name, arguments) \
public:\
static_assert((index) < interfaces_count_limit_ && (index) >= 0, \
            "Interface index out of range!");\
const MemFunctionType<arguments> function_name; \
private:\
template<class T> \
void init_##function_name(T *p) { \
    const_cast<MemFunctionType<arguments>&>(function_name) \
            = MemFunctionType<arguments>::from(p, &T::function_name); \
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
    template<class AnotherType>\
    static void copy(TheClass* theClass, const AnotherType* another) {\
        Copier<(index) - 1, dummy>::copy(theClass, another);\
        const_cast<MemFunctionType<arguments>&>(theClass->function_name) \
            = another->function_name;\
    } \
};

#define INTERFACES_FOOTER \
public: std::shared_ptr<void> getDataPointer() const {return data_;}\
protected: \
static const int defined_interface_count_ \
        = ::__private_helpers::LastNonEmpty<Copier, interfaces_count_limit_ - 1>::last; \
static_assert(defined_interface_count_ > -1, "No interface defined!");\
std::shared_ptr<void> data_; \
template <int index, int dummy> friend struct Copier;\
};

