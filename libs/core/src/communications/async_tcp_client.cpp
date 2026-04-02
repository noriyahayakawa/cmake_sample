#include "communications/async_tcp_client.hpp"

/**
 * @file async_tcp_client.cpp
 * @brief 非同期 TCP クライアントクラスの翻訳単位を提供する。
 */

namespace core::communications {
async_tcp_client::async_tcp_client(asio::io_context &io_context)
    : async_communication(io_context) {}

} // namespace core::communications