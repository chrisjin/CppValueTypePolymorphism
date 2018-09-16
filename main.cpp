#include "ValueTypePoly.h"
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::placeholders;
using namespace std::chrono;
namespace Test {
INTERFACES_HEADER_EX(InterfaceA, 2)
    INTERFACE(0, functionA, void(int a, int b))
    INTERFACE(1, functionB, void(int a, int b))
INTERFACES_FOOTER

INTERFACES_HEADER(InterfaceB)
    INTERFACE(0, functionA, void(int a, int b))
    INTERFACE(1, functionB, void(int a, int b))
    INTERFACE(2, functionC, void(const std::string& value))
INTERFACES_FOOTER
    
INTERFACES_HEADER(InterfaceC)
    INTERFACE(0, combine, std::string(const std::string& a, const std::string& b))
INTERFACES_FOOTER


INTERFACES_HEADER(InterfaceEmpty)
    INTERFACE(0, empty, void())
INTERFACES_FOOTER

// Compiler error: no interface defined
//INTERFACES_HEADER(InterfaceD)
//INTERFACES_FOOTER
}

class InterfaceImpl {
public:
    InterfaceImpl(int val) : val_(val) {
    }
    void functionA(int a, int b) {
        printf("functionA Called. Val: %d\n", val_);
    }
    void functionB(int a, int b) {
        printf("functionB Called. arg: %d,%d\n", a, b);
    }
    void functionC(const std::string& str) {
        printf("functionC Called. str: %s\n", str.c_str());
    }
private:
    int val_;
};

class InterfaceCImpl {
public:
    std::string combine(const std::string& a, const std::string& b) {
        return a + b;
    }
};

class EmptyImpl {
public:
    void empty() {
    }
};

class EmptyVirtual {
public:
    virtual void empty() = 0;
};

class EmptyVirtualImpl : public EmptyVirtual {
public:
    void empty() override {
    }
};
void callInterfaceA(const Test::InterfaceA& a) {
    a.functionA(1, 2);
    a.functionB(3, 4);
}

void callInterfaceB(const Test::InterfaceB& a) {
    a.functionC("Hello");
    a.functionA(1, 2);
    a.functionB(999, 987);
}

INTERFACES_HEADER(Eater)
    INTERFACE(0, eat, void(const std::string& food))
INTERFACES_FOOTER

INTERFACES_HEADER(Speaker)
    INTERFACE(0, speak, void(const std::string& something))
INTERFACES_FOOTER

INTERFACES_HEADER(Walker)
    INTERFACE(0, walk, void())
INTERFACES_FOOTER

INTERFACES_HEADER(WalkerAndEater)
    INTERFACE(0, walk, void())
    INTERFACE(1, eat, void(const std::string& food))
INTERFACES_FOOTER

class Human {
public:
    void eat(const std::string& food) const {
        cout << "Human eat: " << food << endl;
    }
    void drive() const {
        cout << "Human drive" << endl;
    }
    void speak(const std::string& something) const {
        cout << "Human say: " << something << endl;
    }
    void walk() const {
        cout << "Human Walk " << endl;
    }
};

class Monkey {
public:
    void eat(const std::string& food) const {
        cout << "Monkey eat: " << food << endl;
    }
    void walk() const {
        cout << "Monkey Walk " << endl;
    }
};

void eat(const Eater& eater, const std::string& something) {
    eater.eat(something);
}

void walk(const Walker& walker) {
    walker.walk();
}

void speak(const Speaker& speaker, const std::string& something) {
    speaker.speak(something);
}

void walkAndEat(const WalkerAndEater& weater, const std::string& something) {
    cout << " - "; weater.walk();
    cout << " and ..." << endl;
    cout << " - "; weater.eat(something);
}

INTERFACES_HEADER(IncrementalMeter)
    INTERFACE(0, increment, void())
    INTERFACE(1, read, int())
INTERFACES_FOOTER

class IncrementalMeterImpl {
public:
    IncrementalMeterImpl(int v) : value_(v) {}
    void increment() {
        value_++;
    }
    int read() const {
        return value_;
    }
private:
    int value_;
};

class Naming {
public:
    void setName(const std::string& name) {
        name_ = name;
    }
    std::string& getName() {
        return name_;
    }
private:
    std::string name_;
};

INTERFACES_HEADER(ObjectWithName)
    INTERFACE(0, printName, void(Naming& naming))
    INTERFACE(1, setName, void(Naming& naming, const std::string& name))
    INTERFACE(2, getName, std::string& (Naming& naming))
INTERFACES_FOOTER

class ObjectWithNameImpl {
public:
    void printName(Naming& naming) {
        std::cout << "Name: " << naming.getName() << std::endl;
    }
    void setName(Naming& naming, const std::string& name) {
        naming.setName(name);
        std::cout << "Name set to: " << name << std::endl;
    }
    std::string& getName(Naming& naming) {
        return naming.getName();
    }
};

class NotCopyableMonkey {
public:
    NotCopyableMonkey() {}
    void eat(const std::string& something) {
        std::cout << "Not copyable monkey eating " << something << std::endl;
    }
    NotCopyableMonkey(const NotCopyableMonkey&) = delete;
    NotCopyableMonkey& operator=(const NotCopyableMonkey&) = delete;
};

template <typename T, typename Another>
struct IsInterface_ : std::false_type { };

template <typename T>
struct IsInterface_ <T, decltype(&T::INTERFACE_SIGNATURE)> : std::true_type { };

template <typename T>
class IsInterface__
{
    template <typename C> constexpr static bool get(decltype(&C::INTERFACE_SIGNATURE, 'a') k) {
        return true;
    }
    template <typename C> constexpr static bool get(int a) {return false;}
public:
    const static bool value = get<T>('a');
};

class ClassWithoutDefault {
public:
    ClassWithoutDefault(int a) {}
};

template <typename T, typename Another = int>
struct HasFunction : std::false_type { };

template <typename T>
struct HasFunction <T, decltype(&T::getEmpty, 0)> : std::true_type { };


template<class CounterClass, template<int a, int dummy> class T, bool empty, int count>
struct tIfEmpty;

template <class CounterClass, template<int a, int dummy> class T, int count>
struct tLastNonEmpty {
    static const int last = tIfEmpty<CounterClass, T, T<count, 0>::empty, count>::value;
};

template<class CounterClass, template<int a, int dummy> class T, int count>
struct tIfEmpty<CounterClass, T, false, count> {
    static const int value = tLastNonEmpty<CounterClass, T, count + 1>::last;
};

template<class CounterClass, template<int a, int dummy> class T, int count>
struct tIfEmpty<CounterClass, T, true, count> {
    static const int value = count - 1;
};

template<class CounterClass, template<int a, int dummy> class T, bool empty, int count>
struct tIfEmpty1;

template <class CounterClass, template<int a, int dummy> class T, int count>
struct tLastNonEmpty1 {
    static const int last = tIfEmpty1<CounterClass, T, HasFunction<T<count, 0>>::value, count>::value;
};

template<class CounterClass, template<int a, int dummy> class T, int count>
struct tIfEmpty1<CounterClass, T, false, count> {
    static const int value = tLastNonEmpty1<CounterClass, T, count + 1>::last;
};

template<class CounterClass, template<int a, int dummy> class T, int count>
struct tIfEmpty1<CounterClass, T, true, count> {
    static const int value = count - 1;
};

template<int index, int dummy>
struct TestEmptyCheck {
    const static bool empty = true;
};

template<int dummy>
struct TestEmptyCheck<0, dummy> {
    static constexpr bool getEmpty() {
        return false;
    }
    const static bool empty = false;
};
static const auto mm = HasFunction<TestEmptyCheck<1, 0>>::value;
//static int kk1 = 0;
static int kk1 = tLastNonEmpty1<int, TestEmptyCheck, 0>::last;
template<int dummy>
struct TestEmptyCheck<1, dummy> {
    static constexpr bool getEmpty() {
        return false;
    }
    const static bool empty = false;
};
static const auto mm2 = HasFunction<TestEmptyCheck<1, 0>>::value;
//static const auto mm2 = 0;
//static int kk2 = 0;
static int kk2 = tLastNonEmpty1<char, TestEmptyCheck, 0>::last;

template<int dummy>
struct TestEmptyCheck<2, dummy> {
    const static bool empty = false;
};


int main()
{
    std::cout << "=== Function call ===" << std::endl;
    InterfaceImpl inst(2345);
    callInterfaceB(inst);
    callInterfaceA(inst);
    // print:
    //    functionC Called. str: Hello
    //    functionA Called. Val: 2345
    //    functionB Called. arg: 999,987
    //    functionA Called. Val: 2345
    //    functionB Called. arg: 3,4
    std::cout << "=== Constructor from impl ===" << std::endl;
    Test::InterfaceA baseA = inst;
    baseA.functionB(34, 45);
    // print:
    //    functionB Called. arg: 34,45

    std::cout << "=== Assignment operator from another interface ===" << std::endl;
    Test::InterfaceA anotherA;
    anotherA = baseA;
    anotherA.functionA(12, 45);
    anotherA.functionB(334, 445);
    // print:
    //    functionB Called. arg: 334,445

    std::cout << "=== Copy Constructor from another interface ===" << std::endl;
    Test::InterfaceA thirdA(baseA);
    thirdA.functionA(12, 45);
    thirdA.functionB(334, 445);

    std::cout << "=== Impl with return value ===" << std::endl;
    InterfaceCImpl cImpl;
    Test::InterfaceC baseC(cImpl);
    auto combined = baseC.combine("Foo", "Bar");

    std::cout << combined << std::endl;
    // print:
    //    FooBar

    // === Const function memeber ===
    // baseC.combine = [](const std::string& a, const std::string& b) {return "";};
    // Compiler error
    const int count = 10000000;
    EmptyImpl empty;
    Test::InterfaceEmpty emptyInterface(empty);

    auto t1 = steady_clock::now();
    for (int i = 0; i < count; i++) {
        emptyInterface.empty();
    }
    auto t2 = steady_clock::now();
    std::cout << duration_cast<milliseconds>(t2 - t1).count() << std::endl;

    std::function<void()> func = std::bind(&EmptyImpl::empty, &empty);
    auto t5 = steady_clock::now();
    for (int i = 0; i < count; i++) {
        func();
    }
    auto t6 = steady_clock::now();
    std::cout << duration_cast<milliseconds>(t6 - t5).count() << std::endl;

    auto t3 = steady_clock::now();
    EmptyVirtual* baseEmpty = new EmptyVirtualImpl();
    for (int i = 0; i < count; i++) {
        baseEmpty->empty();
    }
    auto t4 = steady_clock::now();
    std::cout << duration_cast<milliseconds>(t4 - t3).count() << std::endl;
    // performance:
    // Value type polymorphism: 69  ms, 34 ms with -O2
    // std::function          : 169 ms, 35 ms with -O2
    // virtual function       : 34  ms, 20 ms with -O2

    Human human;
    Monkey monkey;
    eat(human, "Beef");
    eat(monkey, "Banana");
    speak(human, "Fxxk");
    walk(human);
    walk(monkey);
    walkAndEat(human, "Ramen");
    walkAndEat(monkey, "melon");

    // extract an interface.
    WalkerAndEater weater = human;
    Eater only_eater = weater;
    only_eater.eat("Fried rice");

    std::vector<WalkerAndEater> arr;
    arr.push_back(human);
    arr.push_back(monkey);
    for (int i = 0; i < arr.size(); i++) {
        std::cout << "--";
        arr[i].eat("shit");
    }
    // compiler error. since eater doesn't have all interfaces needed by WalkerAndEater
    // WalkerAndEater weater_super = only_eater;

    // print:
    //    Human eat: Beef
    //    Monkey eat: Banana
    //    Human say: Fxxk
    //    Human Walk
    //    Monkey Walk
    //    - Human Walk
    //    and ...
    //    - Human eat: Ramen
    //    - Monkey Walk
    //    and ...
    //    - Monkey eat: melon
    //    Human eat: Fried rice

    std::cout << "=== Wrap smart pointer ===" << std::endl;

    std::shared_ptr<Human> pHuman = std::make_shared<Human>();
    Walker walker(pHuman);
    walker.walk();
    // Human Walk

    std::cout << "=== Stateful object held by reference ===" << std::endl;
    auto meterInst = make_shared<IncrementalMeterImpl>(0);
    IncrementalMeter meter = meterInst;
    std::cout << meter.read() << ", ";
    meter.increment();
    std::cout << meter.read() << ", ";
    meter.increment();
    std::cout << meter.read() << std::endl;
    // 0, 1, 2

    // the instance is shared by the 2 meters
    meterInst->increment();
    IncrementalMeter secondMeter = meter;
    std::cout << secondMeter.read() << ", ";
    meter.increment();
    std::cout << secondMeter.read() << ", ";
    secondMeter.increment();
    std::cout << secondMeter.read() << ", ";
    meterInst->increment();
    std::cout << secondMeter.read() << endl;
    // 3, 4, 5, 6

    std::cout << "=== Stateful object held by value ===" << std::endl;
    IncrementalMeterImpl meterValue(0);
    IncrementalMeter meterBase = meterValue;
    meterValue.increment();
    std::cout << meterBase.read() << ", ";
    meterBase.increment();
    std::cout << meterBase.read() << endl;
    // 0, 1

    std::cout << "=== Parameters forwarding ===" << std::endl;
    Naming naming;
    ObjectWithName withName = ObjectWithNameImpl();
    withName.setName(naming, "Alice");
    withName.printName(naming);
    withName.getName(naming) = "Bob";
    withName.printName(naming);

    std::cout << "=== not copyable ===" << std::endl;
    // compiler error static_assert failed
    //    NotCopyableMonkey notCopyableMonkey1;
    //    Eater notcopyableEater1 = notCopyableMonkey1;
    // no error with shared_ptr
    Eater notcopyableEater = std::make_shared<NotCopyableMonkey>();
    notcopyableEater.eat("Not copyable banana");
    cout << IsInterface__<ObjectWithName>::value << endl;
    cout << IsInterface__<ObjectWithNameImpl>::value << endl;
    cout << "===" << endl;
    cout << kk1 << endl;
    cout << kk2 << endl;

    cout << tLastNonEmpty<void, TestEmptyCheck, 0>::last << endl;
    cout << mm << endl;
    cout << mm2 << endl;

}
