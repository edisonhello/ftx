#include "rest/client.h"

namespace ftx {

Order::Order(json j)
    : id(j["id"]), market(j["market"]), type(j["type"]), side(j["side"]),
      price(j["price"].get<std::string>()), size(j["size"].get<std::string>()),
      filled_size(j["filledSize"].get<std::string>()),
      remaining_size(j["remainingSize"].get<std::string>()),
      avg_fill_price(j["avgFillPrice"].get<std::string>()),
      created_at(j["createdAt"].get<std::string>()),
      reduce_only(j["reduceOnly"].get<bool>()), ioc(j["ioc"].get<bool>()),
      post_only(j["postOnly"].get<bool>()),
      clientid(j["clientid"].get<bool>()) {
  std::string strStatus = j["status"];
  if (strStatus == "new")
    status = OrderStatus::_new;
  if (strStatus == "open")
    status = OrderStatus::opened;
  if (strStatus == "closed")
    status = OrderStatus::closed;
}

RESTClient::RESTClient()
{
    if (!api_key.empty()) {
        configure();
    }
}

RESTClient::RESTClient(const std::string api_key, const std::string api_secret, const std::string subaccount_name) {
    set_keys(api_key, api_secret, subaccount_name);
}

void RESTClient::set_keys(const std::string api_key, const std::string api_secret, const std::string subaccount_name) {
    this->api_key = api_key;
    this->api_secret = api_secret;
    this->subaccount_name = subaccount_name;

    configure();
}

void RESTClient::configure() {
    http_client.configure(uri, api_key, api_secret, subaccount_name);
    configured = true;
}

json RESTClient::list_futures()
{
    auto response = http_client.get("futures");
    return json::parse(response.body());
}

json RESTClient::list_markets()
{
    auto response = http_client.get("markets");
    return json::parse(response.body());
}

json RESTClient::get_orderbook(const std::string market, int depth)
{
    auto response =
      http_client.get("markets/" + market + "?depth=" + std::to_string(depth));
    return json::parse(response.body());
}

json RESTClient::get_trades(const std::string market)
{
    auto response = http_client.get("markets/" + market + "/trades");
    return json::parse(response.body());
}

json RESTClient::get_account_info()
{
    auto response = http_client.get("account");
    return json::parse(response.body());
}

json RESTClient::get_open_orders()
{
    auto response = http_client.get("orders");
    return json::parse(response.body());
}

Order RESTClient::place_order(const std::string market,
                             const std::string side,
                             double price,
                             double size,
                             bool ioc,
                             bool post_only,
                             bool reduce_only)
{
    json payload = {{"market", market},
                    {"side", side},
                    {"price", price},
                    {"type", "limit"},
                    {"size", size},
                    {"ioc", ioc},
                    {"postOnly", post_only},
                    {"reduceOnly", reduce_only}};
    auto response = http_client.post("orders", payload.dump());
    json result = json::parse(response.body());
    throw_error_if_error(result);
    return result;
}

Order RESTClient::place_order(const std::string market,
                             const std::string side,
                             double size,
                             bool ioc,
                             bool post_only,
                             bool reduce_only)
{
    json payload = {{"market", market},
                    {"side", side},
                    {"price", NULL},
                    {"type", "market"},
                    {"size", size},
                    {"ioc", ioc},
                    {"postOnly", post_only},
                    {"reduceOnly", reduce_only}};
    auto response = http_client.post("orders", payload.dump());
    json result = json::parse(response.body());
    throw_error_if_error(result);
    return result;
}

Order RESTClient::get_order_status(const std::string order_id) {
    auto response = http_client.get("orders/" + order_id);
    std::cout << "get " << response.body() << std::endl;
    return json::parse(response.body());
}

json RESTClient::cancel_order(const std::string order_id)
{
    auto response = http_client.delete_("orders/" + order_id);
    return json::parse(response.body());
}

json RESTClient::get_fills()
{
    auto response = http_client.get("fills");
    return json::parse(response.body());
}

json RESTClient::get_balances()
{
    auto response = http_client.get("wallet/balances");
    return json::parse(response.body());
}

json RESTClient::get_deposit_address(const std::string ticker)
{
    auto response = http_client.get("wallet/deposit_address/" + ticker);
    return json::parse(response.body());
}

}
