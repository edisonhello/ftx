
#include <iostream>

#include "chase_and_make/chase_and_make.hpp"
#include "rest/client.h"
#include "ws/client.h"
#include "util/env.h"
#include <external/json.hpp>
#include <thread>

using json = nlohmann::json;
using Env = util::env::Env;

using namespace std;
using namespace ftx;

Env env;

int main() {
  string api_key = env["API_KEY"];
  string api_secret = env["API_SECRET"];
  string subaccount = env["SUBACCOUNT"];
  
  chase_and_make::ChaseAndMake cnm(api_key, api_secret, subaccount);

  std::this_thread::sleep_for(std::chrono::seconds(3));

  auto res = cnm.make("ETH-PERP", "buy", float50("0.001"));
  
  std::cout << res.size << ' ' << res.price << std::endl;

}
