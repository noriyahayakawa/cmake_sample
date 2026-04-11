#pragma once

/**
 * @file session.hpp
 * @brief TCP セッションの基底クラスを宣言する。
 */

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

namespace comm {

namespace asio = boost::asio;
using boost::asio::ip::tcp;
using boost::signals2::connection;
using boost::signals2::signal;

/**
 * @brief TCP セッションを表す基底クラス。
 * @details
 * 個別の接続ごとのセッション管理を担う基底型。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class session : public std::enable_shared_from_this<session> {
public:
  using ptr = std::shared_ptr<session>;
  using error_signal = signal<void(ptr, boost::system::error_code)>;

  /** @brief 接続先不明時のセッション識別文字列。 */
  const std::string unknown_id = "unknown";

  /**
   * @brief エラー発生時のシグナルスロットを登録する。
   * @param slot 登録するスロット。
   * @return 登録されたコネクションオブジェクト。
   */
  connection on_error(const error_signal::slot_type &slot) {
    return error_signal_.connect(std::move(slot));
  }

  /**
   * @brief コンストラクタ。
   * @param socket 接続済みソケット。
   */
  explicit session(tcp::socket socket) : socket_(std::move(socket)) {}

  /** @brief 仮想デストラクタ。 */
  virtual ~session() = default;

  /**
   * @brief セッション識別文字列を返す。
   * @details
   * IPv4 の場合は "host:port"、IPv6 の場合は "[host]:port" 形式。
   * ゾーン ID の `%` は `%25` にエスケープされる。
   * @return 識別文字列。接続先不明時は `unknown_id`。
   */
  std::string id() const {
    boost::system::error_code ec;
    const auto remote_endpoint = socket_.remote_endpoint(ec);
    if (ec) {
      return unknown_id;
    }
    const auto remote_address = remote_endpoint.address();
    std::string remote_host = remote_address.to_string();
    if (remote_address.is_v6()) {
      for (std::size_t pos = 0;
           (pos = remote_host.find('%', pos)) != std::string::npos; pos += 3) {
        remote_host.replace(pos, 1, "%25");
      }
      remote_host = "[" + remote_host + "]";
    }
    const auto remote_port = remote_endpoint.port();

    return remote_host + ":" + std::to_string(remote_port);
  }

protected:
  /** @brief 接続ソケット。 */
  tcp::socket socket_;

  /**
   * @brief エラーシグナルを発火する。
   * @param ec 発生したエラーコード。
   */
  void emit_error(boost::system::error_code ec) {
    error_signal_(shared_from_this(), ec);
  }

private:
  error_signal error_signal_; ///< エラー発生時に通知するシグナル。
};

} // namespace comm