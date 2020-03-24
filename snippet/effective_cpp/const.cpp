#include <iostream>

class TextBlock {
    public:
        TextBlock(const std::string& str):text_(str) {
        }
        const char& operator[] (std::size_t pos) const {
            return text_[pos];
        }
        char& operator[] (std::size_t pos) {
            return text_[pos];
        }
        const std::string to_string() {
            return text_;
        }
    private:
        std::string text_;
};

int main() {
    TextBlock tb("mytext");
    tb[4] = 'z';
    std::cout << tb.to_string() << std::endl;
    std::cout << tb[4] << std::endl;
    const TextBlock ctb("aaaaaa");
    ctb[0] = '3';
    std::cout << ctb[3] << std::endl;
    return 0;
}
