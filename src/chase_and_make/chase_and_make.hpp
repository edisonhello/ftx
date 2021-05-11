
#pragma once

#include <string>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "rest/client.h"
#include "ws/client.h"

using string = std::string;
using float_50 = boost::multiprecision::cpp_dec_float_50;

namespace chase_and_make {

struct OrderResult {
  float50 size, price;
};

enum OrderSide {
  buy, sell,
};

class ChaseAndMake {
 public:
  ChaseAndMake(const string api_key, const string api_secret, const string subaccount_name);

  OrderResult make(const string pair, const string side, float_50 amount, bool return_at_partial_fill = true);
 private:
  ftx::WSClient ws;
  ftx::RESTClient rest;

  std::shared_ptr<std::promise<ftx::Ticker>> get_ticker(const string pair);
};

} // namespace chase_and_make