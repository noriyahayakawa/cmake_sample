#include "communications/async_tcp_server.hpp"

/**
 * @file async_tcp_server.cpp
 * @brief 非同期 TCP サーバクラスの翻訳単位を提供する。
 */

namespace core::communications {
async_tcp_server::async_tcp_server(asio::io_context &io_context)
    : async_communication(io_context) {}

} // namespace core::communications