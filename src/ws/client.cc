
#include <utility>

#include "util/Encoding.h"
#include "util/Time.h"
#include "ws/client.h"

#undef string_to_hex // avoid define pollusion to OPENSSL_hexstr2buf

namespace encoding = util::encoding;

namespace ftx {

Ticker::Ticker(json j) : market(j["market"]) {
  json jj = j["data"];
  data.bid = float50(jj["bid"].get<double>());
  data.ask = float50(jj["ask"].get<double>());
  data.bid_size = float50(jj["bidSize"].get<double>());
  data.ask_size = float50(jj["askSize"].get<double>());
  data.last = float50(jj["last"].get<double>());
  data.time = jj["time"].get<double>();
}

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
  std::vector<json> msgs;

  if (!(api_key.empty() || api_secret.empty())) {
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

  return msgs;
}

void WSClient::subscribe_orders(std::string market) {
  subscribe(market, "orders");
}

void WSClient::subscribe_orderbook(std::string market) {
  subscribe(market, "orderbook");
}

void WSClient::subscribe_fills(std::string market) {
  subscribe(market, "fills");
}

void WSClient::subscribe_trades(std::string market) {
  subscribe(market, "trades");
}

void WSClient::subscribe_ticker(std::string market) {
  subscribe(market, "ticker");
}

void WSClient::subscribe(std::string market, std::string channel) {
  auto pair = std::make_pair(market, channel);
  if (subscription_set.count(pair))
    return;
  send_message(get_subscription_message(market, channel));
  subscription_set.insert(pair);
}

void WSClient::unsubscribe_orders(std::string market) {
  unsubscribe(market, "orders");
}

void WSClient::unsubscribe_orderbook(std::string market) {
  unsubscribe(market, "orderbook");
}

void WSClient::unsubscribe_fills(std::string market) {
  unsubscribe(market, "fills");
}

void WSClient::unsubscribe_trades(std::string market) {
  unsubscribe(market, "trades");
}

void WSClient::unsubscribe_ticker(std::string market) {
  unsubscribe(market, "ticker");
}

void WSClient::unsubscribe(std::string market, std::string channel) {
  auto pair = std::make_pair(market, channel);
  if (!subscription_set.count(pair))
    return;
  send_message(get_unsubscription_message(market, "orders"));
  subscription_set.erase(pair);
}

void WSClient::send_message(json j) { ws.send_message(j.dump()); }

json WSClient::get_subscription_message(std::string market,
                                        std::string channel) {
  return {{"op", "subscribe"}, {"channel", channel}, {"market", market}};
}

json WSClient::get_unsubscription_message(std::string market,
                                          std::string channel) {
  return {{"op", "unsubscribe"}, {"channel", channel}, {"market", market}};
}

} // namespace ftx
