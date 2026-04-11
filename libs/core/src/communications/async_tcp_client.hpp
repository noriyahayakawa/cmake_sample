#pragma once

/**
 * @file async_tcp_client.hpp
 * @brief core 層の非同期 TCP クライアントクラスを宣言する。
 */

#include "comm/async_tcp_client.hpp"
#include "communications/async_communication.hpp"

namespace core::communications {

/**
 * @brief core 層の非同期 TCP クライアントクラス。
 * @details
 * async_communication を基底として、クライアント固有の動作を提供する。
 */
class async_tcp_client : public async_communication {
public:
  /**
   * @brief コンストラクタ。
   * @param io_context 非同期処理を実行する I/O コンテキスト。
   */
  explicit async_tcp_client(asio::io_context &io_context);

  /** @brief 仮想デストラクタ。 */
  virtual ~async_tcp_client() = default;
};

} // namespace core::communications
