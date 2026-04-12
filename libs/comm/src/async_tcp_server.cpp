#include "comm/async_tcp_server.hpp"
#include "comm/session_manager.hpp"

/**
 * @file async_tcp_server.cpp
 * @brief 非同期 TCP サーバクラスの翻訳単位を提供する。
 */

namespace comm {
async_tcp_server::async_tcp_server(asio::io_context &io_context) {}

boost::asio::awaitable<void>
async_tcp_server::start_accepting(const std::string &service) {
  auto executor = co_await asio::this_coro::executor;
  if (!service.empty()) {
    tcp::resolver resolver(executor);
    auto endpoints =
        co_await resolver.async_resolve("localhost", service,
                                        asio::use_awaitable);
    tcp::acceptor acceptor(executor, *endpoints.begin());

    for (;;) {
      auto [ec, socket] = co_await acceptor.async_accept(
          asio::make_strand(executor),
          asio::as_tuple(asio::use_awaitable));
      if (ec == asio::error::operation_aborted) {
        break;
      }
      if (!ec) {
        auto new_session = std::make_shared<session>(std::move(socket));
        session_manager::instance().add_session(new_session);
      }
    }
  }
}

} // namespace comm