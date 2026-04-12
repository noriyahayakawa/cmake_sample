#pragma once

/**
 * @file async_tcp_server.hpp
 * @brief 非同期 TCP サーバクラスを宣言する。
 */

#include "comm_export.hpp"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

namespace comm {

namespace asio = boost::asio;
using boost::asio::ip::tcp;

/**
 * @brief 非同期 TCP サーバを表すクラス。
 * @details
 * Boost.Asio を利用した非同期 TCP 接続の受け付けと通信を管理する。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class COMM_EXPORT async_tcp_server {
  asio::cancellation_signal cancel_signal_; ///< accept ループのキャンセルシグナル。

public:
  /**
   * @brief コンストラクタ。
   * @param io_context 非同期処理を実行する I/O コンテキスト。
   */
  explicit async_tcp_server(asio::io_context &io_context);

  /** @brief 仮想デストラクタ。 */
  virtual ~async_tcp_server() = default;

  /**
   * @brief 接続の受け付けを開始する。
   * @details
   * `asio::bind_cancellation_slot(cancel_signal_.slot(), asio::detached)` を
   * `co_spawn` の引数に指定することで、`cancel()` によるキャンセルが有効になる。
   * @return 非同期処理を表す awaitable オブジェクト。
   */
  boost::asio::awaitable<void> start_accepting(const std::string &service);

  /**
   * @brief accept ループをキャンセルする。
   * @details `cancellation_type::terminal` をシグナルに発行する。
   */
  void cancel() {
    cancel_signal_.emit(asio::cancellation_type::terminal);
  }

  /**
   * @brief cancel シグナルのスロットを返す。
   * @details `co_spawn` 実行時に `asio::bind_cancellation_slot` へ渡す。
   * @return キャンセルスロット。
   */
  asio::cancellation_slot slot() { return cancel_signal_.slot(); }
};

} // namespace comm