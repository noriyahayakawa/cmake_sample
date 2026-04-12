#include "settings/commons.hpp"

/**
 * @file commons.cpp
 * @brief 共通設定構造体の JSON 変換処理を実装する。
 */

namespace core::settings {

void commons::resolve_relative_path(const boost::filesystem::path &path) {
  if (!output_dir.is_absolute()) {
    output_dir = path / output_dir;
  }
}

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値から `core::settings::commons` を生成する。
 * @param jv 変換元 JSON 値。
 * @return 変換後の `commons` 構造体。
 * @details
 * - `jv` を JSON オブジェクトとして解釈する。
 * - `appName` と `version` をキー存在確認付きで読み取る。
 * - キーが存在しない場合は対応するメンバへ空文字を設定する。
 * - `output_dir` をキー存在確認付きで読み取る。
 * - キーが存在しない場合はデフォルトで `./output` を設定する。
 */
::core::settings::commons
tag_invoke(boost::json::value_to_tag<::core::settings::commons>,
           const value &jv) {
  const object &obj = jv.as_object();
  ::core::settings::commons result;
  result.app_name = obj.if_contains("appName")
                        ? value_to<std::string>(obj.at("appName"))
                        : "";
  result.version = obj.if_contains("version")
                       ? value_to<std::string>(obj.at("version"))
                       : "";
  result.output_dir = obj.if_contains("output_dir")
                          ? value_to<fs::path>(obj.at("output_dir"))
                          : fs::path("./output");
  return result;
}

/**
 * @brief `core::settings::commons` を JSON 値へ変換する。
 * @param jv 変換先 JSON 値。
 * @param commons 変換元の設定値。
 * @details
 * - 出力先は JSON オブジェクトで上書きする。
 * - 空文字でない項目のみ JSON オブジェクトへ出力する。
 */
void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::commons &commons) {
  object obj;
  if (!commons.app_name.empty()) {
    obj["appName"] = value_from(commons.app_name);
  }
  if (!commons.version.empty()) {
    obj["version"] = value_from(commons.version);
  }
  obj["output_dir"] = value_from(commons.output_dir);
  jv = std::move(obj);
}

} // namespace boost::json
