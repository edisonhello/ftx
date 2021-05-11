
#include <mutex>
#include <utility>

#include "util/Encoding.h"
#include "util/Time.h"
#include "util/ftx.hpp"
#include "ws/client.h"

#undef string_to_hex // avoid define pollusion to OPENSSL_hexstr2buf

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
  ws.set_on_message_cb([this] (json j) {
    throw_ftx_exception_if_error(j);

    std::lock_guard<std::mutex> _lg(on_message_callbacks_lock);
    for (auto &[_, cb] : on_message_callbacks) {
      std::thread(cb, j).detach();
    }
  });

  configured = true;
}

int WSClient::on_message(util::WS::OnMessageCB cb) {
  std::lock_guard<std::mutex> _lg(on_message_callbacks_lock);
  return on_message_callbacks.insert(cb);
}

bool WSClient::remove_message_callback(int cb_id) {
  std::lock_guard<std::mutex> _lg(on_message_callbacks_lock);
  return on_message_callbacks.remove(cb_id);
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
      msg["args"].push_back({"subaccount", subaccount_name});
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
