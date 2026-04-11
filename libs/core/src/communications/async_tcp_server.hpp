#pragma once

/**
 * @file async_tcp_server.hpp
 * @brief core 層の非同期 TCP サーバクラスを宣言する。
 */

#include "comm/async_tcp_server.hpp"
#include "communications/async_communication.hpp"

namespace core::communications {

/**
 * @brief core 層の非同期 TCP サーバクラス。
 * @details
 * async_communication を基底として、サーバ固有の動作を提供する。
 */
class async_tcp_server : public async_communication {
public:
  /**
   * @brief コンストラクタ。
   * @param io_context 非同期処理を実行する I/O コンテキスト。
   */
  explicit async_tcp_server(asio::io_context &io_context);

  /** @brief 仮想デストラクタ。 */
  virtual ~async_tcp_server() = default;
};

} // namespace core::communications
