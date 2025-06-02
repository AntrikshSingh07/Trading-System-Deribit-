#include "api_credentials.h"
#include "websocket_handler.h"
#include "trade_execution.h"
#include "latency_module.h"
#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <unordered_map>
#include <future>
#include <vector>
#include <thread>
#include <chrono>
#include <immintrin.h>

// Dummy broadcast function (does nothing, no errors)
void broadcastMessage(const std::string& message) {
    // Future: can be connected to real websocket clients
}

void executeTrades() {
    try {
        WebSocketHandler websocket("test.deribit.com", "443", "/ws/api/v2");
        websocket.connect();

        auto trade = std::make_unique<TradeExecution>(websocket);

        json auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
        bool isAuth = !auth_response.contains("error");

        if (!isAuth) {
            std::cerr << "Authentication failed. Exiting...\n";
            return;
        }

        std::unordered_map<std::string, json> order_cache;

        while (true) {
            int choice;
            std::cout << "\n--- Trading Menu ---\n";
            std::cout << "1. Place Order\n";
            std::cout << "2. Cancel Order\n";
            std::cout << "3. Modify Order\n";
            std::cout << "4. Get Order Book\n";
            std::cout << "5. View Current Positions\n";
            std::cout << "6. Get Live Mark Price\n";
            std::cout << "7. View Open Orders\n";
            std::cout << "8. Exit\n";
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            auto loop_start = LatencyModule::start();

            if (choice == 8) {
                std::cout << "Exiting trading application.\n";
                break;
            }

            switch (choice) {
                case 1: { // Place Order
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    std::string instrument, side;
                    double amount, price;

                    std::cout << "Enter instrument name: ";
                    std::cin >> instrument;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    std::cout << "Enter price: ";
                    std::cin >> price;
                    std::cout << "Enter side (buy/sell): ";
                    std::cin >> side;

                    try {
                        auto order_future = std::async(std::launch::async, [&]() {
                            auto order_start = LatencyModule::start();
                            json response;
                            if (side == "buy") {
                                response = trade->placeBuyOrder(instrument, amount, price);
                            } else if (side == "sell") {
                                response = trade->placeSellOrder(instrument, amount, price);
                            } else {
                                throw std::invalid_argument("Invalid side, must be 'buy' or 'sell'.");
                            }
                            LatencyModule::end(order_start, "Order Placement");
                            return response;
                        });

                        json order_response = order_future.get();
                        std::cout << "Order Response: " << order_response.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error placing order: " << e.what() << std::endl;
                    }
                    break;
                }

                case 2: { // Cancel Order
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    std::string order_id;
                    std::cout << "Enter order ID to cancel: ";
                    std::cin >> order_id;

                    try {
                        auto cancel_future = std::async(std::launch::async, [&]() {
                            auto cancel_start = LatencyModule::start();
                            json cancel_response = trade->cancelOrder(order_id);
                            LatencyModule::end(cancel_start, "Cancel Order");
                            return cancel_response;
                        });

                        json cancel_response = cancel_future.get();
                        std::cout << "Cancel Response: " << cancel_response.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error cancelling order: " << e.what() << std::endl;
                    }
                    break;
                }

                case 3: { // Modify Order
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    std::string order_id;
                    double amount, price;
                    std::cout << "Enter order ID to modify: ";
                    std::cin >> order_id;
                    std::cout << "Enter new amount: ";
                    std::cin >> amount;
                    std::cout << "Enter new price: ";
                    std::cin >> price;

                    try {
                        auto modify_future = std::async(std::launch::async, [&]() {
                            auto modify_start = LatencyModule::start();
                            json modify_response = trade->modifyOrder(order_id, price, amount);
                            LatencyModule::end(modify_start, "Modify Order");
                            return modify_response;
                        });

                        json modify_response = modify_future.get();
                        std::cout << "Modify Response: " << modify_response.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error modifying order: " << e.what() << std::endl;
                    }
                    break;
                }

                case 4: { // Get Order Book
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    std::string instrument;
                    std::cout << "Enter instrument name: ";
                    std::cin >> instrument;

                    try {
                        auto order_book_start = LatencyModule::start();
                        json order_book = trade->getOrderBook(instrument);
                        LatencyModule::end(order_book_start, "Order Book Fetch");

                        std::cout << "Order Book: " << order_book.dump(4) << std::endl;
                        broadcastMessage(order_book.dump());
                    } catch (const std::exception& e) {
                        std::cerr << "Error fetching order book: " << e.what() << std::endl;
                    }
                    break;
                }

                case 5: { // View Current Positions
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    try {
                        json positions = trade->getPositions();
                        std::cout << "Current Positions: " << positions.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error fetching positions: " << e.what() << std::endl;
                    }
                    break;
                }

                case 6: { // Get Live Mark Price
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    std::string instrument;
                    std::cout << "Enter instrument name: ";
                    std::cin >> instrument;

                    try {
                        json ticker = trade->getTicker(instrument);
                        if (ticker.contains("result")) {
                            std::cout << "Mark Price: " << ticker["result"]["mark_price"] << std::endl;
                        } else {
                            std::cout << "Failed to fetch mark price.\n";
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error fetching mark price: " << e.what() << std::endl;
                    }
                    break;
                }

                case 7: { // View Open Orders
                    if (!isAuth) {
                        auth_response = trade->authenticate(CLIENT_ID, CLIENT_SECRET);
                        isAuth = !auth_response.contains("error");
                        if (!isAuth) {
                            std::cout << "Authentication failed.\n";
                            break;
                        }
                    }

                    try {
                        json open_orders = trade->viewOpenOrders();
                        std::cout << "Open Orders: " << open_orders.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error fetching open orders: " << e.what() << std::endl;
                    }
                    break;
                }

                default:
                    std::cout << "Invalid choice. Please try again.\n";
                    break;
            }

            LatencyModule::end(loop_start, "End-to-End Trading Loop Latency");
        }

        websocket.close();
    } catch (const std::exception& e) {
        std::cerr << "Error in executeTrades: " << e.what() << std::endl;
    }
}

int main() {
    try {
        executeTrades();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}
