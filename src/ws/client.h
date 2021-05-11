#pragma once

#include "util/WS.h"
#include "util/ordered_pool.hpp"
#include <external/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace ftx {

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

class WSClient
{
  public:
    WSClient();
    WSClient(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");

    void set_keys(const std::string api_key, const std::string api_secret, const std::string subaccount_name = "");
    void configure();

    int on_message(util::WS::OnMessageCB cb);
    bool remove_message_callback(int cb_id);
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

    std::set<std::pair<std::string, std::string>> subscription_set;
    util::ordered_pool<util::WS::OnMessageCB> on_message_callbacks;

    bool configured = false;

    json get_subscription_message(std::string market, std::string channel);
    json get_unsubscription_message(std::string market, std::string channel);

    void subscribe(std::string market, std::string channel);
    void unsubscribe(std::string market, std::string channel);

    void send_message(json j);
};

}
