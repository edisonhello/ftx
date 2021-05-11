
#include <exception>
#include <stdexcept>
#include <string>

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <external/json.hpp>

#include "util/json.hpp"

namespace ftx {

using json = nlohmann::json;
using float50 = boost::multiprecision::cpp_dec_float_50;

enum OrderStatus {
  _new,
  opened,
  closed,
};

struct Order {
  int64_t id;
  std::string market;
  std::string type;
  std::string side;
  float50 price;
  float50 size;
  float50 filled_size;
  float50 remaining_size;
  float50 avg_fill_price;
  OrderStatus status;
  std::string created_at;
  bool reduce_only;
  bool ioc;
  bool post_only;
  std::string clientid;

  Order() = default;
  Order(json j);
};


struct Ticker {
  std::string market;
  struct {
    float50 bid;
    float50 ask;
    float50 bid_size;
    float50 ask_size;
    float50 last;
    double time;
  } data;

  Ticker() = default;
  Ticker(json j);
};

class ftx_error : public std::runtime_error {
 public:
  ftx_error(std::string msg = "");
};

class action_error : public std::runtime_error {
 public:
  action_error(std::string msg = "");
};

void throw_ftx_exception_if_error(json j);

} // namespace ftx