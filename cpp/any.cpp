#include <iostream>
#include <vector>
#include <any>

struct day { 
  // ...things... 
  void* user_data; 
}; 
 
struct month { 
  std::vector<day> days; 
  void* user_data; 
};

struct Foo {
    void frobnicate() {
        std::cout << "dd: " << a << std::endl;
    }
    int a;
};

int main(){
    std::any a0;
    std::any a1 = 42;
    std::any

}
