#pragma once

/**
 * @file show_help.hpp
 * @brief ヘルプ表示要求例外を宣言する。
 */

#include "core_export.hpp"
#include <boost/exception/all.hpp>
#include <string>

namespace core::exceptions {

/** @brief ヘルプ文言を保持する例外付加情報。 */
using errinfo_message = boost::error_info<struct tag_msg, std::string>;

/**
 * @brief ヘルプ表示要求を表す例外。
 * @details
 * コマンドライン解析中に `-h` または `--help` が指定された際に送出される。
 * 呼び出し側はこの例外を通常エラーとして扱わず、格納されたヘルプ文言を
 * 表示して正常終了できる。
 */
struct CORE_EXPORT show_help : virtual boost::exception,
                               virtual std::exception {
  /** @brief 仮想デストラクタ。 */
  virtual ~show_help() override = default;

  /**
   * @brief 例外メッセージを返す。
   * @details
   * `errinfo_message` が付加されている場合はその文字列を返す。
   * 付加されていない場合は既定値 `"show_help"` を返す。
   * @return 例外メッセージ文字列へのポインタ。
   */
  const char *what() const noexcept override;
};

} // namespace core::exceptions