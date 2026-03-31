#pragma once

/**
 * @file input_file.hpp
 * @brief 入力ファイル全体を表す構造体と JSON 変換関数を宣言する。
 */

#include "settings/commons.hpp"
#include "settings/i_input_data.hpp"
#include <boost/json.hpp>

namespace core::settings {

/**
 * @brief JSON ファイルのルートオブジェクト構造。
 * @details
 * 入力ファイルから読み込まれた JSON ルートの値を保持する。
 * `boost::json` の `tag_invoke` カスタマイゼーションポイントで JSON との
 * 相互変換が可能。
 */
struct input_file : public i_input_data {
  /** @brief アプリケーション共通設定。 */
  commons commons;

  virtual void
  resolve_relative_path(const boost::filesystem::path &path) override {
    commons.resolve_relative_path(path);
  }
};

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値を `core::settings::input_file` に変換する。
 * @param jv 変換元の JSON 値。
 * @return 変換結果の `input_file` 構造体。
 * @details
 * ルートオブジェクトの `commons` キーを `core::settings::commons` として
 * 読み取る。`commons` キーが存在しない場合は既定構築した値を設定する。
 */
::core::settings::input_file
tag_invoke(value_to_tag<::core::settings::input_file>, const value &jv);

/**
 * @brief `core::settings::input_file` を JSON 値に変換する。
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param input_file 変換元の `input_file` 構造体。
 * @details `commons` メンバを `commons` キーとして JSON
 * オブジェクトへ格納する。
 */
void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::input_file &input_file);

} // namespace boost::json