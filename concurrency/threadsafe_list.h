#include <memory>
#include <mutex>
template <typename T>
class threadsafe_list {
    struct node {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<T> next;
        node():next(){};
        node(T const& value):data(std::make_shared<T>(value)){};
    };
    node head;
public:
    threadsafe_list(){};
    ~threadsafe_list(){
        remove_if([](node const&){return true;});
    };
};
