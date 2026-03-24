#pragma once

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>
#include <string>

namespace core::exceptions {

using errinfo_message = boost::error_info<struct tag_msg, std::string>;

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