#include <iostream>

class Rational {
public:
    Rational(int numerator = 0, int denominator = 1) {
        n = numerator;
        d = denominator;
    }
    double getn() {
        return static_cast<double>(this->n) / static_cast<double>(this->d);
    }
private:
    int n, d;
    friend const Rational& operator* (const Rational& lhs,
            const Rational& rhs) {
        Rational result(lhs.n*rhs.n, lhs.d*rhs.d);
        return result;
    }
};

int main() {
    Rational a(1,2);
    Rational b(3,5);
    Rational c = a * b;
    std::cout << c.getn() << std::endl;
    return 0;
}
