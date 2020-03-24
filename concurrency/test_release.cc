#include <iostream>

class A{
public:
    A(int a):a_(a){
        std::cout << "constructing A with " << a_ << std::endl;
    }
    ~A(){
        std::cout << "destructing A with " << a_ << std::endl;
    }

private:
    int a_;
};

int fib(int n) {
    if (n == 0 || n == 1)
        return n;
    return fib(n-1) + fib(n-2);
}
int main(int argc, char* argv[]) {
    A* x = new A(1);
    std::cout << "fib(n): " << fib(45) << std::endl;
    x = new A(2);
    delete x;
    return 0;
}
