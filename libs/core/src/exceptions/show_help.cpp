#include "show_help.hpp"

namespace core::exceptions {

/**
 * @brief ヘルプ表示用メッセージを返す。
 * @details
 * `errinfo_message` が存在する場合はその内容を返し、存在しない場合は
 * 既定値 `"show_help"` を返す。
 * @return 例外メッセージ文字列へのポインタ。
 */
const char *show_help::what() const noexcept {
  if (const std::string *msg = boost::get_error_info<errinfo_message>(*this)) {
    return msg->c_str();
  }
  return "show_help";
}

} // namespace core::exceptions