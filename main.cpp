#include "ValueTypePoly.h"
#include <iostream>
using namespace std;
using namespace std::placeholders;

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
    InterfaceImpl inst(2345);
    callInterfaceB(inst);
    callInterfaceA(inst);
    // print:
    //    functionC Called. str: Hello
    //    functionA Called. Val: 2345
    //    functionB Called. arg: 999,987
    //    functionA Called. Val: 2345
    //    functionB Called. arg: 3,4
    
    Test::InterfaceA baseA = inst;
    baseA.functionB(34, 45);
    Test::InterfaceA anotherA;
    anotherA = inst;
    // print:
    //    functionB Called. arg: 34,45
    InterfaceCImpl cImpl;
    Test::InterfaceC baseC(cImpl);
    auto combined = baseC.combine("Foo", "Bar");
    
    std::cout << combined << std::endl;
    // print:
    //    FooBar
}
