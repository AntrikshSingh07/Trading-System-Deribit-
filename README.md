
# Deribit Trading Application

This project is a C++-based trading application that connects to a cryptocurrency exchange via WebSockets. It enables real-time trade execution, order management, and position monitoring. The system is optimized for responsiveness using asynchronous I/O, efficient memory handling, and multithreading.

## Features

- **Real-Time WebSocket Communication**  
  Maintains a persistent WebSocket connection with the exchange for streaming data and sending orders instantly.

- **Command-Line Interface**  
  Provides a menu-driven terminal interface to perform trading operations easily.

- **Supports Multiple Order Types**  
  Allows placing market, limit, and other supported order types.

- **Secure API Authentication**  
  Handles API credentials securely to enable authenticated access to trading functions.

- **Order Lifecycle Management**  
  View, modify, and cancel active orders directly from the interface.

- **Optimized Performance**  
  Employs asynchronous communication, efficient memory practices, and thread management for fast and responsive performance.

## Project Structure

The application is modular to support clarity, maintenance, and scalability:

### Modules Overview

1. **Main Driver**
   - `deribit_trader.cpp`:  
     Initializes the application and provides the CLI for trading operations.

2. **WebSocket Communication**
   - `websocket_handler.h/cpp`:  
     Manages WebSocket connections, sends/receives messages, handles reconnections.

3. **Trade Operations**
   - `trade_execution.h/cpp`:  
     Implements authenticated trade actions like placing, modifying, and canceling orders. Also fetches account and market data.

4. **Latency Monitoring**
   - `latency_module.h/cpp`:  
     Logs time taken for critical operations for performance benchmarking.

5. **API Credentials**
   - `api_credentials.h`:  
     Stores API keys securely in a centralized and modular format.

## Example Workflow

- **Startup**:  
  The application establishes a WebSocket connection, authenticates using API credentials, and presents a command-line menu.

- **Real-time Actions**:  
  Users can place, cancel, or modify orders, and view the order book or account positions, all in real time via the terminal interface.

## Prerequisites

### System Requirements

- C++17 compatible compiler:
  - GCC 7+ (Linux)
  - Clang 7+ (macOS/Linux)
  - MSVC 2017+ (Windows)
- CMake 3.x or newer
- Boost C++ Libraries
- OpenSSL
- Git

### Dependencies

- [`nlohmann/json`](https://github.com/nlohmann/json) – for JSON parsing
- WebSocket library (e.g. `websocketpp`)
- Boost (`boost::asio` etc.)

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/automatesolutions/WebSocket_HFT.git
cd WebSocket_HFT
```

### 2. Install Dependencies

#### Install Boost

Ubuntu/Debian:
```bash
sudo apt-get install libboost-all-dev
```

macOS (Homebrew):
```bash
brew install boost
```

Windows:  
Download from the [Boost website](https://www.boost.org/).

#### Install OpenSSL

Ubuntu/Debian:
```bash
sudo apt-get install libssl-dev
```

macOS (Homebrew):
```bash
brew install openssl
```

Windows:  
Download from the [OpenSSL website](https://www.openssl.org/).

#### Install Other Dependencies

Using `vcpkg`:

```bash
./vcpkg install nlohmann-json websocketpp
```

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
cmake --build . --config debug
```

### 4. Run the Application

```bash
./deribit_trader.exe
```

## Usage

Upon running, you'll see a command-line menu:

```
--- Trading Menu ---
1. Place Order
2. Cancel Order
3. Modify Order
4. Get Order Book
5. View Current Positions
6. Exit
```

### Example Operations

- **Place Order**  
  Select `1`, enter instrument name, quantity, and price.

- **Cancel Order**  
  Select `2`, provide the order ID.

- **Modify Order**  
  Select `3`, input order ID along with updated values.

- **View Order Book**  
  Select `4`, enter the instrument to retrieve the book.

- **View Positions**  
  Select `5` to see open positions.

- **Exit**  
  Select `6` to quit.

## Optimizations

- **Memory Efficiency**  
  Uses `std::make_unique` for better memory management and safety.

- **Async Network Calls**  
  All communication is non-blocking to ensure application remains responsive.

- **Threading Strategy**  
  Employs `std::async` for concurrent execution of network and computation tasks.

- **Potential SIMD Support**  
  Future iterations may include SIMD or parallel processing for analyzing large order books.

## Future Enhancements

- SIMD-based data processing.
- Multi-exchange support.
- Improved fault tolerance and retry logic for network failures.

## Contributing

Contributions are welcome! Feel free to fork the repository, make changes, and submit a pull request.
