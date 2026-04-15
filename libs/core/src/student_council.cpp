/**
 * @file student_council.cpp
 * @brief student_council の実装を定義する。
 */

#include "core/student_council.hpp"
#include "core/settings/options.hpp"

namespace core {

/**
 * @brief 全役員の挨拶処理を順に呼び出す。
 */
void student_council::hello() const {
  president_.hello();
  vice_president_.hello();
  secretary_.hello();
  historian_.hello();
  treasurer_.hello();
}

/**
 * @brief 生徒会メイン処理を実行する。
 * @details
 * 現在の実装は空で、将来の拡張ポイントとして保持している。
 */
void student_council::run() {
  auto executor_ = asio::this_coro::executor;
  const auto &options = settings::options::instance();
}

} // namespace core