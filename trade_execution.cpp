#include "trade_execution.h"
#include "websocket_handler.h"
#include "latency_module.h"
#include <iostream>
#include <stdexcept>

std::atomic<int> TradeExecution::request_id{1};

TradeExecution::TradeExecution(WebSocketHandler& websocket)
    : websocket_(websocket) {}

TradeExecution::~TradeExecution() {
    market_data_subscribers_.clear();
}

int TradeExecution::getNextRequestId() {
    return request_id++;
}

void TradeExecution::ensureAuthenticated() {
    if (!is_authenticated_) {
        throw std::runtime_error("Operation requires authentication. Please authenticate first.");
    }
}

json TradeExecution::authenticate(const std::string& client_id, const std::string& client_secret) {
    try {
        json auth_request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "public/auth"},
            {"params", {
                {"grant_type", "client_credentials"},
                {"client_id", client_id},
                {"client_secret", client_secret}
            }}
        };
        websocket_.sendMessage(auth_request);
        auto response = websocket_.readMessage();

        if (response.contains("result")) {
            is_authenticated_ = true;
            return response["result"];
        } else {
            throw std::runtime_error("Authentication failed: " + response.dump());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in authenticate: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::getInstruments(const std::string& currency, const std::string& kind, bool expired) {
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "public/get_instruments"},
            {"params", {{"currency", currency}, {"kind", kind}, {"expired", expired}}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in getInstruments: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::getTicker(const std::string& instrument_name) {
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "public/ticker"},
            {"params", {{"instrument_name", instrument_name}}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in getTicker: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::getOrderBook(const std::string& instrument_name) {
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "public/get_order_book"},
            {"params", {{"instrument_name", instrument_name}}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in getOrderBook: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::placeBuyOrder(const std::string& instrument_name, double amount, double price) {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/buy"},
            {"params", {
                {"instrument_name", instrument_name},
                {"amount", amount},
                {"type", "limit"},
                {"price", price}
            }}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in placeBuyOrder: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::placeSellOrder(const std::string& instrument_name, double amount, double price) {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/sell"},
            {"params", {
                {"instrument_name", instrument_name},
                {"amount", amount},
                {"type", "limit"},
                {"price", price}
            }}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in placeSellOrder: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::cancelOrder(const std::string& order_id) {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/cancel"},
            {"params", {{"order_id", order_id}}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in cancelOrder: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::modifyOrder(const std::string& order_id, double new_price, double new_amount) {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/edit"},
            {"params", {
                {"order_id", order_id},
                {"new_price", new_price},
                {"new_amount", new_amount},
                {"contracts", new_amount}
            }}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in modifyOrder: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::getPositions() {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/get_positions"},
            {"params", {}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in getPositions: " << e.what() << std::endl;
        throw;
    }
}

json TradeExecution::viewOpenOrders(const std::string& currency) {
    ensureAuthenticated();
    try {
        json request = {
            {"jsonrpc", "2.0"},
            {"id", getNextRequestId()},
            {"method", "private/get_open_orders_by_currency"},
            {"params", {{"currency", currency}}}
        };
        websocket_.sendMessage(request);
        return websocket_.readMessage();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in viewOpenOrders: " << e.what() << std::endl;
        throw;
    }
}

void TradeExecution::handleMarketData(const json& data) {
    if (data.contains("symbol")) {
        std::string symbol = data["symbol"];
        if (market_data_subscribers_.count(symbol)) {
            market_data_subscribers_[symbol](data);
        } else {
            std::cerr << "No subscribers for symbol: " << symbol << std::endl;
        }
    } else {
        std::cerr << "Invalid market data: Missing 'symbol'" << std::endl;
    }
}

void TradeExecution::onMarketDataReceived(const json& market_data) {
    auto start = LatencyModule::start();
    handleMarketData(market_data);
    LatencyModule::end(start, "Market Data Processing Latency");
}

void TradeExecution::broadcastDummy() {
    std::cout << "Broadcasting dummy message (no-op)." << std::endl;
}

void TradeExecution::addMarketDataSubscriber(const std::string& symbol, std::function<void(const json&)> callback) {
    market_data_subscribers_[symbol] = callback;
}
