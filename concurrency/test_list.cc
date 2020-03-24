#include <list>
#include <utility>
#include <iostream>
#include <string>
using std::string;

typedef std::pair<string, string> V;
typedef std::list<V> D;
typedef typename D::iterator I;
typedef typename D::const_iterator CI;

I find_entry(D& d, string const& key) {
    CI i = std::find_if(d.begin(), d.end(), 
            [&](V const& item) {return item.first == key;});
    I res = d.begin();
    std::advance(res, std::distance<ConstIter>(res, i));
    return res;
}

int main() {
    D d;
    auto key = "a";
    auto val = "b";
    d.push_back(V(key, val));
    I i = std::find_if(d.begin(), d.end(), 
            [&](V const& item) {return item.first == key;});
    if(i != d.end()) {
        std::cout << i->first << i->second << std::endl;
        i->second = "c";
    }
    for(I i = d.begin(); i != d.end(); ++i) {
        std::cout << i->first << i->second << std::endl;
    }
    return 0;
}
