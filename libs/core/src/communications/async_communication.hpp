#pragma once

/**
 * @file async_communication.hpp
 * @brief 非同期通信の基底クラスを宣言する。
 */

#include "comm/async_tcp_server.hpp"
#include "core/settings/options.hpp"
#include <boost/asio/awaitable.hpp>

namespace core::communications {

namespace asio = boost::asio;
using asio::ip::tcp;

/**
 * @brief 非同期 TCP 通信の基底クラス。
 * @details
 * io_context と設定オブジェクトを保持し、名前解決などの共通機能を提供する。
 */
class async_communication {
protected:
  asio::io_context &io_context_;         ///< 非同期処理を実行する I/O コンテキスト。
  core::settings::options &options_;     ///< アプリケーション設定のシングルトン。

public:
  /**
   * @brief コンストラクタ。
   * @param io_context 非同期処理を実行する I/O コンテキスト。
   */
  explicit async_communication(asio::io_context &io_context);

  /** @brief 仮想デストラクタ。 */
  virtual ~async_communication() = default;

  /**
   * @brief ホスト名とサービス名を非同期で解決する。
   * @param host    解決するホスト名。
   * @param service 解決するサービス名またはポート番号。
   * @return 解決済みエンドポイントのリスト。
   */
  asio::awaitable<tcp::resolver::results_type>
  resolve_once(std::string host, std::string service);
};

} // namespace core::communications
