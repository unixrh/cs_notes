#include <iostream>
#include <utility>
using std::ostream;

void x(const std::string& a) {
    std::cout << std::endl;
    std::cout << a << std::endl;
}

template<typename T>
ostream &print(ostream &os, const T &t) {
    return os << t;
}

template<typename T, typename... Args>
ostream &print(ostream &os, const T &t, Args&&... rest) {
    os << t << ", ";
    x(std::forward<Args>(rest)...);
    return print(os, rest...);
}

int main() {
    print(std::cout, "2", 4, 'a', 9, 0, "afasfaf");
}
