
#include "rest/client.h"
#include "ws/client.h"
#include "util/env.h"
#include <external/json.hpp>

using json = nlohmann::json;
using Env = util::env::Env;

using namespace std;
using namespace ftx;

Env env;

#include <iostream>

int main() {
  string api_key = env["API_KEY"];
  string api_secret = env["API_SECRET"];
  string subaccount = env["SUBACCOUNT"];
  
  RESTClient rest(api_key, api_secret, subaccount);
  WSClient ws(api_key, api_secret, subaccount);
  
  ws.subscribe_ticker("BTC-PERP");
  ws.subscribe_ticker("BTC-1231");
  ws.subscribe_ticker("BTC-0624");
  ws.subscribe_ticker("ETH-PERP");
  ws.subscribe_ticker("ETH-1231");
  ws.subscribe_ticker("ETH-0624");

  ws.on_message([] (json j) {
    cout << "msg: " << j << endl;
  });

  ws.connect();
  
  // auto futs = client.list_futures();
  // std::cout << futs << "\n";
  
  // auto markets = client.list_futures();
  // std::cout << markets << "\n";
  
  // auto ob = client.get_orderbook("BTC-PERP", 3);
  // std::cout << ob << "\n\n";
  
  // auto acct = client.get_account_info();
  // std::cout << acct << "\n\n";
  
  // auto ord = client.place_order("BTC-PERP", "buy", 10, 0.01);
  // std::cout << ord << "\n\n";
}



// int main()
// {
//     client.subscribe_orders("BTC-PERP");
//     client.subscribe_orderbook("BTC-PERP");
//     client.subscribe_ticker("BTC-PERP");

//     client.on_message([](json j) { std::cout << "msg: " << j << "\n"; });

//     client.connect();
// }
