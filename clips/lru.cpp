/* LRUCache: 使用list + unordered_map实现。
             list: 存储一组kv组成的pair
             unordered_map: key为key，value为list的迭代器
             put: map中已有，则将它从map和list中删除
                  组一个pair，插入map并且插入到list的前端
                  若超过容量，则删除list的最后一个元素
             get: 若map中没有则抛出异常
                  否则返回val，然后将当前的kv pair移到list的前端

*/
#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

template <typename key_t, typename val_t>
class LRUCache {
 public:
  typedef typename std::pair<key_t, val_t> kv_pair_t;
  typedef typename list<kv_pair_t>::iterator list_iterator_t;

  LRUCache(size_t cap): _cap(cap) {
  }

  void put(const key_t &k, const val_t &v) {
    auto it = _map.find(k);
    _list.push_front(kv_pair_t(k, v));

    if (it != _map.end()) {
      _list.erase(it->second);
      _map.erase(it);
    }

    _map[k] = _list.begin();

    if (_map.size() > _cap) {
      auto last = _list.rbegin();
      _map.erase(last->first);
      _list.pop_back();
    }
  }

  val_t get(const key_t &k) {
    auto it = _map.find(k);

    if (it == _map.end()) {
      throw std::range_error("there is no such key");
    } else {
      _list.splice(_list.begin(), _list, it->second);
      return it->second->second;
    }
  }

  bool exists(key_t k) {
    return _map.find(k) != _map.end();
  }

  size_t size() {
    return _map.size();
  }

  void display() {
    for (auto const &item : _list) {
      cout << item.first << ":" << item.second << "\t";
    }

    cout << endl;
  }

 private:
  list<kv_pair_t> _list;
  unordered_map<key_t, list_iterator_t> _map;
  int _cap;
};

int main() {
  LRUCache<string, string> cache(4);
  cache.put("a", "a1");
  cache.put("b", "b1");
  cache.put("c", "c1");
  cache.put("c", "c2");
  cache.put("d", "d2");
  cache.put("d", "d1");
  cache.put("e", "e1");
  cout << cache.get("c") << endl;
  cout << cache.get("b") << endl;
  cache.display();
}
