#include <iostream>
class T {
public:
    T();
    ~T(){
        std::cout << "~base\n";
        log();
    }
    virtual void log() const {
        std::cout << "base\n";
    }
};
T::T() {
    log();
}

class bT: public T {
public:
    virtual void log() const override {
        std::cout << "bt\n";
    }
};

class aT: public T {
public:
    virtual void log() const {
        std::cout << "at\n";
    }
};

void dod(T& t) {
    t.log();
}

int main() {
    aT a;
    dod(a);
    return 0;
}
