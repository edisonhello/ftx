#pragma once

#include "util/WS.h"
#include <external/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace ftx {

struct Ticker {
  std::string market;
  cpp_dec_float_50 bid, ask, bid_size, ask_size, last;
  double time;
};

class WSClient
{
  public:
    WSClient();
    WSClient(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");

    void set_keys(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");
    void configure();

    void on_message(util::WS::OnMessageCB cb);
    void connect();
    std::vector<json> on_open();

    void subscribe_orders(std::string market);
    void subscribe_orderbook(std::string market);
    void subscribe_fills(std::string market);
    void subscribe_trades(std::string market);
    void subscribe_ticker(std::string market);

    void unsubscribe_orders(std::string market);
    void unsubscribe_orderbook(std::string market);
    void unsubscribe_fills(std::string market);
    void unsubscribe_trades(std::string market);
    void unsubscribe_ticker(std::string market);

  private:
    std::vector<std::pair<std::string, std::string>> subscriptions;
    util::WS::OnMessageCB message_cb;
    util::WS ws;
    const std::string uri = "wss://ftx.com/ws/";
    std::string api_key = "";
    std::string api_secret = "";
    std::string subaccount_name = "";

    bool configured = false;

    json get_subscription_message(std::string market, std::string channel);
    json get_unsubscription_message(std::string market, std::string channel);

    void send_message(json j);
};

}
