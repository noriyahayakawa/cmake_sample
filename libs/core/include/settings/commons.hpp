#pragma once

/**
 * @file commons.hpp
 * @brief 共通設定構造体と JSON 変換関数を宣言する。
 */

#include "settings/i_input_data.hpp"
#include <boost/json.hpp>
#include <string>

namespace core::settings {

/**
 * @brief アプリケーション共通設定を保持する構造体。
 * @details
 * 入力 JSON の `commons` オブジェクトに対応する値オブジェクト。
 * `boost::json` の `tag_invoke` カスタマイゼーションポイントにより JSON 値との
 * 相互変換ができる。
 */
struct commons : public i_input_data {
  /** @brief アプリケーション名 */
  std::string appName;
  /** @brief アプリケーションのバージョン文字列 */
  std::string version;

  virtual void
  resolve_relative_path(const boost::filesystem::path &path) override {}
};

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値を `core::settings::commons` に変換する。
 * @param jv 変換元の JSON 値。
 * @return 変換結果の `commons` 構造体。
 * @details
 * `jv` は JSON オブジェクトであることを想定し、`appName` と `version` を
 * 読み取る。キーが存在しない場合は対応するメンバへ空文字を設定する。
 */
::core::settings::commons tag_invoke(value_to_tag<::core::settings::commons>,
                                     const value &jv);

/**
 * @brief `core::settings::commons` を JSON 値に変換する。
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param commons 変換元の `commons` 構造体。
 * @details 空文字の項目は JSON 出力に含めない。
 */
void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::commons &commons);

} // namespace boost::json