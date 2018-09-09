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
}
