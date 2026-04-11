#pragma once

/**
 * @file session_manager.hpp
 * @brief TCP セッション管理クラスを宣言する。
 */

#include "comm/session.hpp"
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/serialization/singleton.hpp>
#include <map>
#include <memory>
#include <string>

namespace comm {

/**
 * @brief 複数の TCP セッションを管理するクラス。
 * @details
 * 接続セッションの生成・保持・破棄を一元管理する。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class session_manager : private boost::noncopyable {
private:
  friend class boost::serialization::singleton<session_manager>;
  friend class boost::serialization::detail::singleton_wrapper<session_manager>;
  std::map<std::string, std::shared_ptr<session>> sessions_;

  /** @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。 */
  session_manager() = default;

public:
  static session_manager &instance() {
    return boost::serialization::singleton<
        session_manager>::get_mutable_instance();
  }

  virtual ~session_manager() = default;

  /** @brief セッションを追加する。 */
  void add_session(std::shared_ptr<session> new_session) {
    sessions_.emplace(new_session->id(), std::move(new_session));
    new_session->on_error(
        [](session::ptr session, boost::system::error_code ec) {
          session_manager::instance().remove_session(session);
        });
  }

  /**
   * @brief セッションを削除する。
   * @param session 削除するセッション。
   */
  void remove_session(std::shared_ptr<session> session) {
    sessions_.erase(session->id());
  }
};

} // namespace comm