
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
  string pair1, pair2, side, size_type;
  float50 size1, size2;
};

void print_help(string program_name) {
  cerr << "Usage: " << program_name << " "
       << "<pair1> <pair2> <buy/sell> <sizetype:USD/coin> <size1> "
          "<size2>"
       << endl;
}

Config get_setting(int argc, char **argv) {
  if (argc != 7) {
    print_help(argv[0]);
    exit(1);
  }

  Config config{argv[1], argv[2],          argv[3],
                argv[4], float50(argv[5]), float50(argv[6])};

  if (config.side != "buy" && config.side != "sell") {
    print_help(argv[0]);
    exit(1);
  }

  if (config.size_type != "USD" && config.size_type != "coin") {
    print_help(argv[0]);
    exit(1);
  }

  return config;
}

string op_side(string side) { return side == "buy" ? "sell" : "buy"; }

auto convert_size(const Market &m1, const Market &m2, float50 s1,
                  float50 s2) {
  s1 /= m1.last;
  s2 /= m2.last;
  return pair{s1, s2};
}

int main(int argc, char **argv) {
  auto [pair1, pair2, side, size_type, size1, size2] =
      get_setting(argc, argv);

  string api_key = env["API_KEY"];
  string api_secret = env["API_SECRET"];
  string subaccount = env["SUBACCOUNT"];

  RESTClient rest("", "", "");
  Market market1 = rest.get_market(pair1);
  Market market2 = rest.get_market(pair2);

  if (size_type == "USD") {
    cout << "converting USD to coin amount" << endl;

    tie(size1, size2) = convert_size(market1, market2, size1, size2);

    cout << "pair1 last price " << market1.last << endl;
    cout << "convert to size " << size1 << endl;

    cout << "pair2 last price " << market2.last << endl;
    cout << "convert to size " << size2 << endl;
  }

  size1 = round_size(market1, size1);
  size2 = round_size(market2, size2);

  cout << "will " << side << " " << pair1 << " for amount " << size1
       << " (USD: " << size1 * market1.last << ")" << endl;
  cout << "will " << (side == "buy" ? "sell" : "buy") << " " << pair2
       << " for amount " << size2 << " (USD: " << size2 * market2.last
       << ")" << endl;

  cout << "confirm (press enter)" << endl;
  cin.ignore();

  chase_and_make::ChaseAndMake cnm(api_key, api_secret, subaccount);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  float50 avg_price_1 = 0, avg_price_2 = 0;
  float50 size1_left = size1, size2_left = size2;
  while (size1_left > 0) {
    auto res1 = cnm.make(pair1, side, size1_left, true);
    avg_price_1 += res1.filled_size * res1.avg_fill_price;
    size1_left -= res1.filled_size;
    cout << "filled " << pair1 << " size " << res1.filled_size << " at "
         << res1.avg_fill_price << endl;

    float50 tmp_size2 = round_size(market2, size2_left - size1_left);
    auto res2 = cnm.make(pair2, op_side(side), tmp_size2, false);
    avg_price_2 += res2.filled_size * res2.avg_fill_price;
    size2_left -= res2.filled_size;
    cout << "filled " << pair2 << " size " << res2.filled_size << " at "
         << res2.avg_fill_price << endl;
  }

  avg_price_1 /= size1;
  avg_price_2 /= size2;

  cout << "done" << endl;
  cout << pair1 << " avg filled at " << avg_price_1 << endl;
  cout << pair2 << " avg filled at " << avg_price_2 << endl;
}
