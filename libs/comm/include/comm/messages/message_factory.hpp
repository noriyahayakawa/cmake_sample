#pragma once

/**
 * @file message_factory.hpp
 * @brief TCP メッセージのファクトリークラスを宣言する。
 */

#include "comm/messages/message.hpp"
#include <boost/core/noncopyable.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/function.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace comm::messages {

/**
 * @brief TCP メッセージを生成するシングルトンファクトリークラス。
 *
 * メッセージ型の文字列キーに対応するクリエーター関数を管理し、
 * キーを指定して `message` の派生オブジェクトを生成する。
 *
 * @note コピー不可。インスタンスは `instance()` を通じて取得する。
 */
class message_factory : private boost::noncopyable {
  friend class boost::serialization::singleton<message_factory>;
  friend class boost::serialization::detail::singleton_wrapper<message_factory>;

  /** @brief コンストラクタ（シングルトンのみ構築可能）。 */
  message_factory() = default;

public:
  /**
   * @brief シングルトンインスタンスを返す。
   * @return message_factory の唯一のインスタンスへの参照。
   */
  static message_factory &instance() {
    return boost::serialization::singleton<
        message_factory>::get_mutable_instance();
  }

  /** @brief デストラクタ。 */
  virtual ~message_factory() = default;

  /**
   * @brief 指定した型のメッセージオブジェクトを生成する。
   * @param type メッセージ型を表す文字列キー。
   * @return 生成された `message` 派生オブジェクトの shared_ptr。
   * @throws std::runtime_error 未登録の型を指定した場合。
   */
  std::shared_ptr<message> create_message(const std::string &type);

  /**
   * @brief メッセージ型とそのクリエーター関数を登録する。
   * @param type     メッセージ型を表す文字列キー。
   * @param creator  `message` 派生オブジェクトを生成するファクトリー関数。
   */
  void register_message(const std::string &type, boost::function<std::shared_ptr<message>()> creator);

private:
  /** @brief 型文字列からクリエーター関数へのマップ。 */
  std::unordered_map<std::string, boost::function<std::shared_ptr<message>()>> creators_;
};

} // namespace comm::messages