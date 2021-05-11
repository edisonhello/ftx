
#include "util/ftx.hpp"
#include <exception>
#include <stdexcept>

namespace ftx {

Order::Order(json j)
    : id(j["result"]["id"].get<int64_t>()),
      market(j["result"]["market"].get<std::string>()),
      type(j["result"]["type"].get<std::string>()),
      side(j["result"]["side"].get<std::string>()),
      price(j["result"]["price"].get<double>()),
      size(j["result"]["size"].get<double>()),
      filled_size(j["result"]["filledSize"].get<double>()),
      remaining_size(j["result"]["remainingSize"].get<double>()),
      avg_fill_price(safe_get<double>(j["result"]["avgFillPrice"], 0)),
      created_at(j["result"]["createdAt"].get<std::string>()),
      reduce_only(j["result"]["reduceOnly"].get<bool>()),
      ioc(j["result"]["ioc"].get<bool>()),
      post_only(j["result"]["postOnly"].get<bool>()),
      clientid(safe_get<std::string>(j["result"]["clientid"], "")) {
  std::string strStatus = j["result"]["status"];
  if (strStatus == "new")
    status = OrderStatus::_new;
  if (strStatus == "open")
    status = OrderStatus::opened;
  if (strStatus == "closed")
    status = OrderStatus::closed;
}

Ticker::Ticker(json j) : market(j["market"]) {
  json jj = j["data"];
  data.bid = float50(jj["bid"].get<double>());
  data.ask = float50(jj["ask"].get<double>());
  data.bid_size = float50(jj["bidSize"].get<double>());
  data.ask_size = float50(jj["askSize"].get<double>());
  data.last = float50(jj["last"].get<double>());
  data.time = jj["time"].get<double>();
}



void throw_ftx_exception_if_error(json j) {
  if (j["success"].is_boolean() && j["success"].get<bool>() == false) {
    throw action_error(j["error"].get<std::string>());
  }

  if (j["type"].is_string() && j["type"].get<std::string>() == "error") {
    int code = j["code"].get<int>();
    std::string msg = j["msg"].get<std::string>();

    throw ftx_error(std::to_string(code) + ": " + std::move(msg));
  }
}

ftx_error::ftx_error(std::string msg) : std::runtime_error(msg) {}
action_error::action_error(std::string msg) : std::runtime_error(msg) {}

}