
#include <utility>

#include "ws/client.h"
#include "util/Encoding.h"
#include "util/Time.h"

namespace encoding = util::encoding;

namespace ftx {

WSClient::WSClient() {
  if (!api_key.empty()) {
    configure();
  }
}

WSClient::WSClient(const std::string api_key, const std::string api_secret,
                   const std::string subaccount_name) {
  set_keys(api_key, api_secret, subaccount_name);
}

void WSClient::set_keys(const std::string api_key, const std::string api_secret,
                        const std::string subaccount_name) {
  this->api_key = api_key;
  this->api_secret = api_secret;
  this->subaccount_name = subaccount_name;

  configure();
}

void WSClient::configure() {
  ws.configure(uri, api_key, api_secret, subaccount_name);
  ws.set_on_open_cb([this]() { return this->on_open(); });

  configured = true;
}

void WSClient::on_message(util::WS::OnMessageCB cb) {
  ws.set_on_message_cb(cb);
}

void WSClient::connect() { ws.connect(); }

std::vector<json> WSClient::on_open() {
  std::cout << "1" << std::endl;
  std::vector<json> msgs;
  std::cout << "1" << std::endl;

  if (!(api_key.empty() || api_secret.empty())) {
      std::cout << "login msg" << std::endl;
    long ts = util::get_ms_timestamp(util::current_time()).count();
    std::string data = std::to_string(ts) + "websocket_login";
    std::string hmacced = encoding::hmac(std::string(api_secret), data, 32);
    std::string sign =
        encoding::string_to_hex((unsigned char *)hmacced.c_str(), 32);
    json msg = {{"op", "login"},
                {"args", {{"key", api_key}, {"sign", sign}, {"time", ts}}}};
    if (!subaccount_name.empty()) {
      msg.push_back({"subaccount", subaccount_name});
    }
    msgs.push_back(msg);

  }
  std::cout << "3" << std::endl;

  msgs.push_back(get_subscription_message("BTC-PERP", "orders"));

  return msgs;
}

void WSClient::subscribe_orders(std::string market) {
    send_message(get_subscription_message(market, "orders"));
}

void WSClient::subscribe_orderbook(std::string market) {
    send_message(get_subscription_message(market, "orderbook"));
}

void WSClient::subscribe_fills(std::string market) {
    send_message(get_subscription_message(market, "fills"));
}

void WSClient::subscribe_trades(std::string market) {
    send_message(get_subscription_message(market, "trades"));
}

void WSClient::subscribe_ticker(std::string market) {
    send_message(get_subscription_message(market, "ticker"));
}

void WSClient::unsubscribe_orders(std::string market) {
    send_message(get_unsubscription_message(market, "orders"));
}

void WSClient::unsubscribe_orderbook(std::string market) {
    send_message(get_unsubscription_message(market, "orderbook"));
}

void WSClient::unsubscribe_fills(std::string market) {
    send_message(get_unsubscription_message(market, "fills"));
}

void WSClient::unsubscribe_trades(std::string market) {
    send_message(get_unsubscription_message(market, "trades"));
}

void WSClient::unsubscribe_ticker(std::string market) {
    send_message(get_unsubscription_message(market, "ticker"));
}
// TODO: remove these repeat shits

void WSClient::send_message(json j) {
    ws.send_message(j.dump());
}

json WSClient::get_subscription_message(std::string market,
                                        std::string channel) {
  return {{"op", "subscribe"}, {"channel", channel}, {"market", market}};
}

json WSClient::get_unsubscription_message(std::string market,
                                        std::string channel) {
  return {{"op", "unsubscribe"}, {"channel", channel}, {"market", market}};
}

} // namespace ftx
