```cpp
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
```
