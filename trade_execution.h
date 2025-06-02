#ifndef TRADE_EXECUTION_H
#define TRADE_EXECUTION_H

#include <nlohmann/json.hpp>
#include <string>
#include <functional>
#include <map>
#include <atomic>

using json = nlohmann::json;

// ⬇️ Forward declaration
class WebSocketHandler;

class TradeExecution {
public:
    explicit TradeExecution(WebSocketHandler& websocket);
    ~TradeExecution();

    json authenticate(const std::string& client_id, const std::string& client_secret);

    json getInstruments(const std::string& currency, const std::string& kind, bool expired);
    json getTicker(const std::string& instrument_name);
    json getOrderBook(const std::string& instrument_name);

    json placeBuyOrder(const std::string& instrument_name, double amount, double price);
    json placeSellOrder(const std::string& instrument_name, double amount, double price);
    json cancelOrder(const std::string& order_id);
    json modifyOrder(const std::string& order_id, double new_price, double new_amount);
    json getPositions();
    json viewOpenOrders(const std::string& currency);

    void handleMarketData(const json& data);
    void onMarketDataReceived(const json& market_data);

    void broadcastDummy();
    void addMarketDataSubscriber(const std::string& symbol, std::function<void(const json&)> callback);

private:
    WebSocketHandler& websocket_;
    static std::atomic<int> request_id;
    std::map<std::string, std::function<void(const json&)>> market_data_subscribers_;
    bool is_authenticated_ = false;

    int getNextRequestId();
    void ensureAuthenticated();
};

#endif // TRADE_EXECUTION_H
