
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
  if (strStatus == "new") status = OrderStatus::_new;
  if (strStatus == "open") status = OrderStatus::opened;
  if (strStatus == "closed") status = OrderStatus::closed;
}

Ticker::Ticker(json j)
    : bid(j["bid"].get<double>()),
      ask(j["ask"].get<double>()),
      bid_size(j["bidSize"].get<double>()),
      ask_size(j["askSize"].get<double>()),
      last(j["last"].get<double>()),
      time(j["time"].get<double>()) {}

Market::Market(json j)
    : name(j["name"].get<std::string>()),
      underlying(safe_get<std::string>(j["underlying"], "")),
      base_currency(safe_get<std::string>(j["baseCurrency"], "")),
      quote_currency(safe_get<std::string>(j["quotaCurrency"], "")),
      enabled(j["enabled"].get<bool>()),
      ask(j["ask"].get<double>()),
      bid(j["bid"].get<double>()),
      last(j["last"].get<double>()),
      post_only(j["postOnly"].get<bool>()),
      price_increment(j["priceIncrement"].get<double>()),
      size_increment(j["sizeIncrement"].get<double>()) {}

ftx_error::ftx_error(std::string msg) : std::runtime_error(msg) {}
action_error::action_error(std::string msg) : std::runtime_error(msg) {}

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

float50 prev_size(const Market &market, float50 size) {
  float50 _size = round_size(market, size);
  if (_size >= size) _size -= market.size_increment;
  return _size;
}

float50 round_size(const Market &market, float50 size) {
  size /= market.size_increment;
  size = round(size);
  size *= market.size_increment;
  return size;
}

float50 next_size(const Market &market, float50 size) {
  float50 _size = round_size(market, size);
  if (_size <= size) _size += market.size_increment;
  return _size;
}

float50 prev_price(const Market &market, float50 price) {
  float50 _price = round_price(market, price);
  if (_price >= price) _price -= market.price_increment;
  return _price;
}

float50 round_price(const Market &market, float50 price) {
  price /= market.price_increment;
  price = round(price);
  price *= market.price_increment;
  return price;
}

float50 next_price(const Market &market, float50 price) {
  float50 _price = round_price(market, price);
  if (_price <= price) _price += market.price_increment;
  return _price;
}

}  // namespace ftx
