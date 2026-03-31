#pragma once

/**
 * @file my_error.hpp
 * @brief アプリケーション共通エラー例外と付加情報型を宣言する。
 */

#include <boost/exception/all.hpp>
#include <string>

namespace core::exceptions {

/** @brief 関連するファイルパス文字列を保持する例外付加情報。 */
using errinfo_path = boost::error_info<struct tag_path, std::string>;
/** @brief 関連する errno 相当の整数値を保持する例外付加情報。 */
using errinfo_errno = boost::error_info<struct tag_errno, int>;
/** @brief 利用者向けの説明メッセージを保持する例外付加情報。 */
using errinfo_message = boost::error_info<struct tag_msg, std::string>;

/**
 * @brief アプリケーション固有の汎用エラー例外。
 * @details
 * `errinfo_path`、`errinfo_errno`、`errinfo_message` などの
 * Boost.Exception 情報を付与して送出するための型。
 * `what()` は保持している診断情報から表示用文字列を組み立てて返す。
 */
struct my_error : virtual boost::exception, virtual std::exception {
  mutable std::string what_message_;

  /**
   * @brief 例外メッセージを返す。
   * @details
   * `boost::diagnostic_information` により診断文字列を生成する。
   * 生成結果が空の場合は既定値 `"my_error"` を返す。
   * 文字列生成中に例外が起きた場合は失敗理由を返す。
   * @return NUL 終端文字列へのポインタ。
   */
  const char *what() const noexcept override;
};

} // namespace core::exceptions