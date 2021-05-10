#include "ws/client.h"
#include <chrono>
#include <external/json.hpp>
#include <iostream>
#include <thread>
#include "util/env.h"

using Env = util::env::Env;

using json = nlohmann::json;
using namespace std;

Env env;

int main()
{
    string api_key = env["API_KEY"];
    string api_secret = env["API_SECRET"];
    string subaccount = env["SUBACCOUNT"];

    ftx::WSClient client(api_key, api_secret, subaccount);


    client.on_message([](json j) { std::cout << "msg: " << j << "\n"; });
    client.connect();

    // client.subscribe_orderbook("BTC-PERP");
    // client.subscribe_ticker("BTC-PERP");

    std::this_thread::sleep_for(std::chrono::seconds(10));
    client.subscribe_ticker("BTC-1231");
    std::this_thread::sleep_for(std::chrono::seconds(10));
}
