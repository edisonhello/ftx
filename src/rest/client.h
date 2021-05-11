#pragma once

#include <string>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <external/json.hpp>
#include "util/HTTP.h"
#include "util/ftx.hpp"

using json = nlohmann::json;

namespace ftx {

class RESTClient
{
  public:
    RESTClient();
    RESTClient(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");

    void set_keys(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");

    void configure();

    json list_futures();

    json list_markets();

    json get_orderbook(const std::string market, int depth = 100);

    json get_trades(const std::string market);

    json get_account_info();

    json get_open_orders();

    Order place_order(const std::string market,
                     const std::string side,
                     double price,
                     double size,
                     bool ioc = false,
                     bool post_only = false,
                     bool reduce_only = false);

    // Market order overload
    Order place_order(const std::string market,
                     const std::string side,
                     double size,
                     bool ioc = false,
                     bool post_only = false,
                     bool reduce_only = false);

    Order get_order_status(const std::string order_id);

    json cancel_order(const std::string order_id);

    json get_fills();

    json get_balances();

    json get_deposit_address(const std::string ticker);

  private:
    util::HTTPSession http_client;
    const std::string uri = "ftx.com";
    std::string api_key = "";
    std::string api_secret = "";
    std::string subaccount_name = "";

    bool configured = false;
};

}
