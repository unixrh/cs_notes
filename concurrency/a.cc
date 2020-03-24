#include <iostream>

int main(){
    int a = 5;
    int b = 0;
    while(1) {
        if(b++ < a) {
            std::cout << b << std::endl;
        }
    }
}
