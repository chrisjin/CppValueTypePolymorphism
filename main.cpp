#include "ValueTypePoly.h"
#include <iostream>
#include <chrono>
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
    void functionA(int a, int b) const {
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

INTERFACES_HEADER(Driver)
    INTERFACE(0, drive, void())
INTERFACES_FOOTER

INTERFACES_HEADER(Speaker)
    INTERFACE(0, speak, void(const std::string& something))
INTERFACES_FOOTER

INTERFACES_HEADER(Walker)
    INTERFACE(0, walk, void())
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

void drive(const Driver& driver) {
    driver.drive();
}

void walk(const Walker& walker) {
    walker.walk();
}
void speak(const Speaker& speaker, const std::string& something) {
    speaker.speak(something);
}

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
    // Value type polymorphism: 133 ms, 34 ms with -O2
    // std::function          : 169 ms, 35 ms with -O2
    // virtual function       : 34  ms, 20 ms with -O2

    Human human;
    Monkey monkey;
    eat(human, "Beef");
    eat(monkey, "Banana");
    speak(human, "Fxxk");
    drive(human);
    walk(human);
    walk(monkey);
    // print:
    //    Human eat: Beef
    //    Monkey eat: Banana
    //    Human say: Fxxk
    //    Human drive
    //    Human Walk
    //    Monkey Walk
}
