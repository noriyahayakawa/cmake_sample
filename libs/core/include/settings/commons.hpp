#pragma once

#include <boost/json.hpp>
#include <string>

namespace core::settings {

/**
 * @brief アプリケーション共通設定を保持する構造体
 * @details JSON
 * ファイルから読み込まれ、アプリ全体で共有される設定値を格納する。 boost::json
 * の tag_invoke カスタマイゼーションポイントによって JSON
 * オブジェクトとの相互変換が可能。
 */
struct commons {
  /** @brief アプリケーション名 */
  std::string appName;
  /** @brief アプリケーションのバージョン文字列 */
  std::string version;
};

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値を core::settings::commons に変換する tag_invoke オーバーロード
 * @param jv 変換元の JSON 値。"appName" および "version"
 * キーを持つオブジェクトであること。
 * @return 変換結果の commons 構造体
 * @throws std::invalid_argument jv
 * がオブジェクト型でない場合、またはキーが存在しない場合
 */
::core::settings::commons tag_invoke(value_to_tag<::core::settings::commons>,
                                     const value &jv);

/**
 * @brief core::settings::commons を JSON 値に変換する tag_invoke オーバーロード
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param commons 変換元の commons 構造体
 */
void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::commons &commons);

} // namespace boost::json