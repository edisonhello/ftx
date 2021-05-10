
#include <future>

#include "chase_and_make/chase_and_make.hpp"
#include "rest/client.h"
#include "ws/client.h"

namespace chase_and_make {

ChaseAndMake::ChaseAndMake(const string api_key, const string api_secret, const string subaccount_name) {
  ws.set_keys(api_key, api_secret, subaccount_name);
  rest.set_keys(api_key, api_secret, subaccount_name);
}

OrderResult ChaseAndMake::make(const string pair, const string side, float_50 amount, bool return_at_partial_fill) {
  while (amount > 0) {

    std::future<ftx::Ticker> ticker_future =
        std::async(std::launch::async,
                   [this, pair]() -> ftx::Ticker { return get_ticker(pair); });

    float_50 previous_price = 0;
    ftx::Order previous_order;

    while (true) {
      if (std::future_status status =
              ticker_future.wait_for(std::chrono::seconds(0));
          status == std::future_status::ready) {
        ftx::Ticker ticker = ticker_future.get();

        float_50 this_price;
        if (side == "buy") {
          this_price = ticker.bid;
        } else {
          this_price = ticker.ask;
        }

        if (this_price != previous_price) {
          json cancel_result =
              rest.cancel_order(std::to_string(previous_order.id));
          ftx::Order canceled_order =
              rest.get_order_status(std::to_string(previous_order.id));

          amount -= canceled_order.filled_size;

          if (return_at_partial_fill) {
            return {canceled_order.filled_size, canceled_order.avg_fill_price};
          }

          ftx::Order order =
              rest.place_order(pair, side, this_price.convert_to<double>(),
                               amount.convert_to<double>());
        }

        ticker_future =
            std::async(std::launch::async, [this, pair]() -> ftx::Ticker {
              return get_ticker(pair);
            });
      }
    }
  }
  return {0, 0};

  // TODO: ws listen on fill
}

// ftx::Ticker ChaseAndMake::get_ticker(const std::string pair) {
//   ws.subscribe_ticker(pair);
// }
  
}
