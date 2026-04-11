#pragma once

/**
 * @file async_tcp_client.hpp
 * @brief 非同期 TCP クライアントクラスを宣言する。
 */

#include "comm/session.hpp"
#include <boost/asio.hpp>

namespace comm {

namespace asio = boost::asio;
using boost::asio::ip::tcp;

/**
 * @brief 非同期 TCP クライアントを表すクラス。
 * @details
 * Boost.Asio を利用した非同期 TCP 接続クライアント。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class async_tcp_client : public session {
public:
  /**
   * @brief コンストラクタ。
   * @param io_context 非同期処理を実行する I/O コンテキスト。
   */
  explicit async_tcp_client(asio::io_context &io_context);

  /** @brief 仮想デストラクタ。 */
  virtual ~async_tcp_client() = default;
};

} // namespace comm