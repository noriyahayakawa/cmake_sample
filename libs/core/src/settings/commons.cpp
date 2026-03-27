#include "settings/commons.hpp"

namespace core::settings {}

namespace boost::json {

/**
 * @brief JSON 値から `core::settings::commons` を生成する。
 * @param jv 変換元 JSON 値。
 * @return 変換後の `commons` 構造体。
 * @details
 * - `appName` と `version` をキー存在確認付きで読み取る。
 * - キーが存在しない場合は空文字を設定する。
 */
::core::settings::commons
tag_invoke(boost::json::value_to_tag<::core::settings::commons>,
           const value &jv) {
  const object &obj = jv.as_object();
  ::core::settings::commons commons;
  commons.appName = obj.if_contains("appName")
                        ? value_to<std::string>(obj.at("appName"))
                        : "";
  commons.version = obj.if_contains("version")
                        ? value_to<std::string>(obj.at("version"))
                        : "";
  return commons;
}

/**
 * @brief `core::settings::commons` を JSON 値へ変換する。
 * @param jv 変換先 JSON 値。
 * @param commons 変換元の設定値。
 * @details
 * - 空文字でない項目のみ JSON オブジェクトへ出力する。
 */
void tag_invoke(value_from_tag, value &jv,
                const ::core::settings::commons &commons) {
  object obj;
  if (!commons.appName.empty()) {
    obj.emplace("appName", commons.appName);
  }
  if (!commons.version.empty()) {
    obj.emplace("version", commons.version);
  }
  jv = std::move(obj);
}

} // namespace boost::json
