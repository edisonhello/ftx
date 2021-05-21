
#include <external/json.hpp>
#include <iostream>
#include <thread>

#include "module/chase_and_make/chase_and_make.hpp"
#include "rest/client.h"
#include "util/env.h"
#include "ws/client.h"

using json = nlohmann::json;
using Env = util::env::Env;

using namespace std;
using namespace ftx;

Env env;

struct Config {
  string pair, side;
  float50 size;
  bool ropf;
};

Config get_setting(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Usage: " << argv[0]
         << " <pair> <buy/sell> <size> <return on partial fill>" << endl;
    exit(1);
  }

  return {argv[1], argv[2], float50(argv[3]), string(argv[4]) == "true"};
}

int main(int argc, char **argv) {
  auto [pair, side, size, ropf] = get_setting(argc, argv);

  string api_key = env["API_KEY"];
  string api_secret = env["API_SECRET"];
  string subaccount = env["SUBACCOUNT"];

  chase_and_make::ChaseAndMake cnm(api_key, api_secret, subaccount);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto res = cnm.make(pair, side, size, ropf);

  std::cout << "Filled size: " << res.filled_size
            << " avg fill price: " << res.avg_fill_price << std::endl;
}
