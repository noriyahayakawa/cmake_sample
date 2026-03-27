#pragma once

#include <boost/exception/all.hpp>
#include <string>

namespace core::exceptions {

using errinfo_path = boost::error_info<struct tag_path, std::string>;
using errinfo_errno = boost::error_info<struct tag_errno, int>;
using errinfo_message = boost::error_info<struct tag_msg, std::string>;

/**
 * @brief アプリケーション固有の汎用エラー例外。
 * @details
 * Boost.Exception の情報を付与して送出するための型。
 * `what()` は付与済み情報を含む診断文字列を組み立てて返す。
 */
struct my_error : virtual boost::exception, virtual std::exception {
  mutable std::string what_message_;

  /**
   * @brief 例外メッセージを返す。
   * @details
   * `boost::diagnostic_information` により診断文字列を生成し、生成結果が空なら
   * 既定値 `"my_error"`
   * を返す。文字列生成中に例外が起きた場合は失敗理由を返す。
   * @return NUL 終端文字列へのポインタ。
   */
  const char *what() const noexcept override;
};

} // namespace core::exceptions