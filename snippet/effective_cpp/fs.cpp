#include <iostream>
class FS {
    public:
        FS(){}
        FS(std::size_t a):a_(a){}
        std::size_t numDisks() const {
            return a_;
        }
    private:
        std::size_t a_;
};
FS& tfs() {
    static FS fs(10);
    return fs;
}

class D {
    public:
        D() {
            std::size_t s = tfs().numDisks();
            std::cout << s << std::endl;
        }
};

int main() {
    D d;
    return 0;
}
