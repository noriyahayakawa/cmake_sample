#include "settings/input_file.hpp"

/**
 * @file input_file.cpp
 * @brief 入力ファイル構造体の JSON 変換処理を実装する。
 */

namespace core::settings {
void input_file::resolve_relative_path(const boost::filesystem::path &path) {
  commons.resolve_relative_path(path);
  communications.resolve_relative_path(path);
}
} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値から `core::settings::input_file` を生成する。
 * @param json_value 変換元 JSON 値。
 * @return 変換後の `input_file` 構造体。
 * @details
 * - `json_value` を JSON オブジェクトとして解釈する。
 * - `commons` キーが存在する場合は `core::settings::commons` へ変換する。
 * - `commons` キーが存在しない場合は既定構築した値を設定する。
 */
auto tag_invoke([[maybe_unused]] value_to_tag<::core::settings::input_file> tag,
                const value &json_value) -> ::core::settings::input_file {
  const object &obj = json_value.as_object();
  ::core::settings::input_file input_file;
  input_file.commons =
      (obj.if_contains("commons") != nullptr)
          ? value_to<::core::settings::commons>(obj.at("commons"))
          : ::core::settings::commons{};
  input_file.communications =
      (obj.if_contains("communications") != nullptr)
          ? value_to<::core::settings::communications>(obj.at("communications"))
          : ::core::settings::communications{};
  return input_file;
}

/**
 * @brief `core::settings::input_file` を JSON 値へ変換する。
 * @param json_value 変換先 JSON 値。
 * @param input_file 変換元の入力ファイル構造体。
 * @details
 * - 出力先は JSON オブジェクトで上書きする。
 * - `commons` メンバは対応する `tag_invoke` を使って JSON 値へ変換する。
 * - `communications` メンバは対応する `tag_invoke` を使って JSON 値へ変換する。
 */
void tag_invoke([[maybe_unused]] value_from_tag tag, value &json_value,
                const ::core::settings::input_file &input_file) {
  object obj;
  obj["commons"] = value_from(input_file.commons);
  obj["communications"] = value_from(input_file.communications);
  json_value = std::move(obj);
}

} // namespace boost::json