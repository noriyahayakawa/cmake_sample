#include "core/exceptions/show_help.hpp"

/**
 * @file show_help.cpp
 * @brief ヘルプ表示要求例外のメッセージ取得処理を実装する。
 */

namespace core::exceptions {

/**
 * @brief ヘルプ表示用メッセージを返す。
 * @details
 * `errinfo_message` が存在する場合はその内容を返す。
 * 付加情報がない場合は既定値 `"show_help"` を返す。
 * @return 例外メッセージ文字列へのポインタ。
 */
const char *show_help::what() const noexcept {
  if (const std::string *msg = boost::get_error_info<errinfo_message>(*this)) {
    return msg->c_str();
  }
  return "show_help";
}

} // namespace core::exceptions