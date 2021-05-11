
#include <iterator>
#include <map>

namespace util {

template<typename T>
class ordered_pool {
 public:
  ordered_pool() : index(0) {}

  typename std::map<int, T>::iterator begin() {
    return map.begin();
  }

  typename std::map<int, T>::iterator end() {
    return map.end();
  }

  int insert(T x) {
    int index = get_index();
    map.insert(make_pair(index, x));
    return index;
  }

  // int insert(T &x) {
  //   int index = get_index();
  //   map.insert(index, x);
  //   return index;
  // }

  // int insert(T &&x) {
  //   int index = get_index();
  //   map.insert(index, std::move(x));
  //   return index;
  // }

  bool remove(int index) {
    auto it = map.find(index);
    if (it == map.end()) return false;
    map.erase(it);
    return true;
  }
 private:
  std::map<int, T> map;
  int index;

  int get_index() {
    return ++index;
  }
};

}