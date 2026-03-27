#pragma once

#include <boost/exception/all.hpp>
#include <string>

namespace core::exceptions {

using errinfo_message = boost::error_info<struct tag_msg, std::string>;

/**
 * @brief ヘルプ表示要求を表す例外。
 * @details
 * オプション解析で `--help` が指定された際に送出される。
 * 必要に応じて `errinfo_message` に表示文言を付与できる。
 */
struct show_help : virtual boost::exception, virtual std::exception {
  /**
   * @brief 例外メッセージを返す。
   * @details errinfo_message が付加されている場合はその文字列を返す。
   *          付加されていない場合は "show_help" を返す。
   * @return 例外メッセージ文字列へのポインタ。
   */
  const char *what() const noexcept override;
};

} // namespace core::exceptions