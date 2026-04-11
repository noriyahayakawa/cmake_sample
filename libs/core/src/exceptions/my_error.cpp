#include "core/exceptions/my_error.hpp"

/**
 * @file my_error.cpp
 * @brief アプリケーション共通エラー例外のメッセージ生成処理を実装する。
 */

namespace core::exceptions {

/**
 * @brief 例外内容を診断文字列として返す。
 * @details
 * 保持している Boost.Exception 情報を `boost::diagnostic_information` で
 * 診断文字列へ変換する。
 * 組み立て処理が失敗した場合は失敗理由を返し、最終フォールバックとして
 * `"my_error"` を返す。
 * @return 例外メッセージ文字列へのポインタ。
 */
const char *my_error::what() const noexcept {
  try {
    what_message_ = boost::diagnostic_information(*this);
    if (!what_message_.empty()) {
      return what_message_.c_str();
    }
  } catch (const std::exception &error) {
    what_message_ =
        "failed to build diagnostic_information in my_error::what(): ";
    what_message_ += error.what();
    return what_message_.c_str();
  } catch (...) {
    what_message_ =
        "failed to build diagnostic_information in my_error::what(): "
        "unknown exception";
    return what_message_.c_str();
  }
  what_message_ = "my_error";
  return what_message_.c_str();
}

} // namespace core::exceptions