
#include <external/json.hpp>


namespace ftx {

using json = nlohmann::json;

template<typename T> 
T safe_get(json j, T default_value) {
  return j.is_null() ? default_value : j.get<T>();
}

}