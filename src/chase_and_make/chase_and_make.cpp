
#include <future>
#include <mutex>

#include "chase_and_make/chase_and_make.hpp"
#include "rest/client.h"
#include "ws/client.h"

const bool Verbose = true;

#define log Verbose && std::cout

namespace chase_and_make {

ChaseAndMake::ChaseAndMake(const string api_key, const string api_secret,
                           const string subaccount_name)
    : ws(api_key, api_secret, subaccount_name),
      rest(api_key, api_secret, subaccount_name) {

  // ws.on_message([](json j) { std::cout << "recv: " << j << std::endl; });
  ws.connect();
}

OrderResult ChaseAndMake::make(const string pair, const string side,
                               float_50 amount, bool return_at_partial_fill) {
  while (amount > 0) {

    std::future<ftx::Ticker> ticker_future = get_ticker(pair);

    float_50 previous_price = 0;
    ftx::Order previous_order{};

    while (true) {
      if (std::future_status status =
              ticker_future.wait_for(std::chrono::seconds(1));
          status == std::future_status::ready) {

        ftx::Ticker ticker = ticker_future.get();

        float_50 this_price;
        if (side == "buy") {
          this_price = ticker.data.bid;
        } else {
          this_price = ticker.data.ask;
        }

        if (this_price != previous_price) {
          if (previous_order.id) {
            log << "cancel previous order" << std::endl;

            json cancel_result =
                rest.cancel_order(std::to_string(previous_order.id));
            ftx::Order canceled_order =
                rest.get_order_status(std::to_string(previous_order.id));

            amount -= canceled_order.filled_size;

            if (canceled_order.filled_size) {
              log << "partial filled " << canceled_order.filled_size << " at "
                  << canceled_order.avg_fill_price << std::endl;

              if (return_at_partial_fill) {
                return {canceled_order.filled_size,
                        canceled_order.avg_fill_price};
              }
            }

          }

          ftx::Order order =
              rest.place_order(pair, side, this_price.convert_to<double>(),
                               amount.convert_to<double>());
          previous_price = this_price;

          log << "place new order with size " << amount << " at " << this_price << std::endl;

          previous_order = order;
        }

        ticker_future = get_ticker(pair);
      }
    }
  }
  // TODO: change this;
  return {0, 0};

  // TODO: ws listen on fill
}

std::future<ftx::Ticker> ChaseAndMake::get_ticker(const std::string pair) {
  std::packaged_task<ftx::Ticker()> task([this, pair]() {
    ws.subscribe_ticker(pair);

    std::promise<ftx::Ticker> promise;
    std::mutex mutex;
    bool is_set = false;

    int cb_id = ws.on_message([this, pair, &promise, &mutex, &is_set, &cb_id] (json j) {
      if (j["type"].get<std::string>() == "subscribed") return;
      if (j["channel"].get<std::string>() == "ticker" && j["market"].get<std::string>() == pair) {
        std::lock_guard<std::mutex> _lg(mutex);

        if (!is_set) {
          is_set = true;
          promise.set_value(j);

          ws.remove_message_callback(cb_id);
        }
      }
    });

    return promise.get_future().get();
  });

  task();

  return task.get_future();
  // TODO: maybe we dont need packaged task here
}

} // namespace chase_and_make
