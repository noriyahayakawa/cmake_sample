/**
 * @file student_council.hpp
 * @brief 生徒会シングルトンを宣言する。
 */

#pragma once

#include "historian.hpp"
#include "president.hpp"
#include "secretary.hpp"
#include "treasurer.hpp"
#include "vice_president.hpp"
#include <boost/asio.hpp>
#include <boost/serialization/singleton.hpp>

namespace core {

namespace asio = boost::asio;

/**
 * @brief 生徒会役員群を保持して操作するシングルトンクラス。
 * @details
 * 役員インスタンスを内部に保持し、`hello()` で各役員処理を順に呼び出す。
 */
class CORE_EXPORT student_council : private boost::noncopyable {
private:
  friend class boost::serialization::singleton<student_council>;
  friend class boost::serialization::detail::singleton_wrapper<student_council>;

  asio::io_context io_context_;   ///< 非同期処理の実行コンテキスト。
  president president_;           ///< 生徒会長インスタンス。
  vice_president vice_president_; ///< 副会長インスタンス。
  secretary secretary_;           ///< 書記インスタンス。
  historian historian_;           ///< 記録係インスタンス。
  treasurer treasurer_;           ///< 会計インスタンス。

  /** @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。 */
  student_council() = default;

public:
  /** @brief デストラクタ。 */
  ~student_council() = default;

  /**
   * @brief シングルトンインスタンスへの参照を返す。
   * @return `student_council` の唯一インスタンス。
   */
  static student_council &instance() {
    return boost::serialization::singleton<
        student_council>::get_mutable_instance();
  }

  /**
   * @brief 全役員の挨拶処理を順に実行する。
   */
  void hello() const;

  /**
   * @brief 生徒会のメイン処理を実行する。
   * @details
   * 現状の実装は空で、将来の拡張ポイントとして定義されている。
   */
  asio::awaitable<void> run();
};

} // namespace core