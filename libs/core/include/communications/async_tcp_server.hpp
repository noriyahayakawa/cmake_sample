#pragma once

/**
 * @file async_tcp_server.hpp
 * @brief 非同期 TCP サーバクラスを宣言する。
 */

#include "communications/async_communication.hpp"
#include <boost/asio.hpp>

namespace core::communications {

/**
 * @brief 非同期 TCP サーバを表すクラス。
 * @details
 * Boost.Asio を利用した非同期 TCP 接続の受け付けと通信を管理する。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class async_tcp_server : public async_communication {
public:
  explicit async_tcp_server(asio::io_context &io_context);

  virtual ~async_tcp_server() = default;
};

} // namespace core::communications