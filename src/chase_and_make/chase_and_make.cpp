
#include <future>

#include "chase_and_make/chase_and_make.hpp"
#include "rest/client.h"
#include "ws/client.h"

namespace chase_and_make {

ChaseAndMake::ChaseAndMake(const string api_key, const string api_secret,
                           const string subaccount_name)
    : ws(api_key, api_secret, subaccount_name),
      rest(api_key, api_secret, subaccount_name) {

  ws.on_message([](json j) { std::cout << "recv: " << j << std::endl; });
  ws.connect();
}

OrderResult ChaseAndMake::make(const string pair, const string side,
                               float_50 amount, bool return_at_partial_fill) {
  while (amount > 0) {

    std::shared_ptr<std::promise<ftx::Ticker>> ticker_promise = get_ticker(pair);
    std::future<ftx::Ticker> ticker_future = ticker_promise->get_future();

    float_50 previous_price = 0;
    ftx::Order previous_order;

    while (true) {
      std::cout << "start wait for future" << std::endl;
      if (std::future_status status =
              ticker_future.wait_for(std::chrono::seconds(1));
          status == std::future_status::ready) {
        std::cout << "trying to get value" << std::endl;

        ftx::Ticker ticker = ticker_future.get();

        float_50 this_price;
        if (side == "buy") {
          this_price = ticker.data.bid;
        } else {
          this_price = ticker.data.ask;
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

        ticker_promise = get_ticker(pair);
        ticker_future = ticker_promise->get_future();
      }
    }
  }
  return {0, 0};

  // TODO: ws listen on fill
}

std::shared_ptr<std::promise<ftx::Ticker>> ChaseAndMake::get_ticker(const std::string pair) {
  ws.subscribe_ticker(pair);

  std::shared_ptr<std::promise<ftx::Ticker>> promise(new std::promise<ftx::Ticker>());

  ws.on_message([pair, &promise] (json j) {
    std::cout << "ws on message " << j.dump() << std::endl;
    if (j["type"].get<std::string>() == "subscribed") return;
    if (j["channel"].get<std::string>() == "ticker" && j["market"].get<std::string>() == pair) {
      std::cout << "set value" << std::endl;
      promise->set_value(j);
    }
  });

  return promise;
}

} // namespace chase_and_make
