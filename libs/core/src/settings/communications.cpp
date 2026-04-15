#include <algorithm>

#include "settings/communications.hpp"

/**
 * @file communications.cpp
 * @brief 通信設定構造体の JSON 変換処理を実装する。
 */

namespace core::settings {

void server_settings::resolve_relative_path(
    const boost::filesystem::path &path) {}

void client_settings::resolve_relative_path(
    const boost::filesystem::path &path) {}

void communications::resolve_relative_path(
    const boost::filesystem::path &path) {
  server.resolve_relative_path(path);
  std::ranges::for_each(clients, [&path](client_settings &client) -> void {
    client.resolve_relative_path(path);
  });
}

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値から `core::settings::server_settings` を生成する。
 * @param json_value 変換元 JSON 値。
 * @return 変換後の `server_settings` 構造体。
 * @details
 * - `json_value` を JSON オブジェクトとして解釈する。
 * - `enable`、`id`、`service` をキー存在確認付きで読み取る。
 * - キーが存在しない場合は既定値（`false` または空文字）を設定する。
 */
auto tag_invoke(
    [[maybe_unused]] value_to_tag<::core::settings::server_settings> tag,
    const value &json_value) -> ::core::settings::server_settings {
  const object &obj = json_value.as_object();
  ::core::settings::server_settings result;
  result.enable = (obj.if_contains("enable") != nullptr)
                      ? value_to<bool>(obj.at("enable"))
                      : false;
  result.id = (obj.if_contains("id") != nullptr)
                  ? value_to<std::string>(obj.at("id"))
                  : "";
  result.service = (obj.if_contains("service") != nullptr)
                       ? value_to<std::string>(obj.at("service"))
                       : "";
  return result;
}

/**
 * @brief `core::settings::server_settings` を JSON 値へ変換する。
 * @param json_value 変換先 JSON 値。JSON オブジェクトとして上書きされる。
 * @param server_settings 変換元のサーバ設定。
 * @details
 * - 出力先は JSON オブジェクトで上書きする。
 * - `enable`、`id`、`service` をすべて JSON オブジェクトへ出力する。
 */
void tag_invoke([[maybe_unused]] value_from_tag tag, value &json_value,
                const ::core::settings::server_settings &server_settings) {
  object obj;
  obj["enable"] = value_from(server_settings.enable);
  obj["id"] = value_from(server_settings.id);
  obj["service"] = value_from(server_settings.service);
  json_value = std::move(obj);
}

/**
 * @brief JSON 値から `core::settings::client_settings` を生成する。
 * @param json_value 変換元 JSON 値。
 * @return 変換後の `client_settings` 構造体。
 * @details
 * - `json_value` を JSON オブジェクトとして解釈する。
 * - `enable`、`id`、`host`、`service` をキー存在確認付きで読み取る。
 * - キーが存在しない場合は既定値（`false` または空文字）を設定する。
 */
auto tag_invoke(
    [[maybe_unused]] value_to_tag<::core::settings::client_settings> tag,
    const value &json_value) -> ::core::settings::client_settings {
  const object &obj = json_value.as_object();
  ::core::settings::client_settings result;
  result.enable = (obj.if_contains("enable") != nullptr)
                      ? value_to<bool>(obj.at("enable"))
                      : false;
  result.id = (obj.if_contains("id") != nullptr)
                  ? value_to<std::string>(obj.at("id"))
                  : "";
  result.host = (obj.if_contains("host") != nullptr)
                    ? value_to<std::string>(obj.at("host"))
                    : "";
  result.service = (obj.if_contains("service") != nullptr)
                       ? value_to<std::string>(obj.at("service"))
                       : "";
  return result;
}

/**
 * @brief `core::settings::client_settings` を JSON 値へ変換する。
 * @param json_value 変換先 JSON 値。JSON オブジェクトとして上書きされる。
 * @param client_settings 変換元のクライアント設定。
 * @details
 * - 出力先は JSON オブジェクトで上書きする。
 * - `enable`、`id`、`host`、`service` をすべて JSON オブジェクトへ出力する。
 */
void tag_invoke([[maybe_unused]] value_from_tag tag, value &json_value,
                const ::core::settings::client_settings &client_settings) {
  object obj;
  obj["enable"] = value_from(client_settings.enable);
  obj["id"] = value_from(client_settings.id);
  obj["host"] = value_from(client_settings.host);
  obj["service"] = value_from(client_settings.service);
  json_value = std::move(obj);
}

/**
 * @brief JSON 値から `core::settings::communications` を生成する。
 * @param json_value 変換元 JSON 値。
 * @return 変換後の `communications` 構造体。
 * @details
 * - `json_value` を JSON オブジェクトとして解釈する。
 * - `server` キーが存在する場合は `server_settings` へ変換する。
 * - `clients` キーが存在する場合は `client_settings` の配列へ変換する。
 * - 各キーが存在しない場合は既定構築した値を設定する。
 */
auto tag_invoke(
    [[maybe_unused]] value_to_tag<::core::settings::communications> tag,
    const value &json_value) -> ::core::settings::communications {
  const object &obj = json_value.as_object();
  ::core::settings::communications result;
  result.server =
      (obj.if_contains("server") != nullptr)
          ? value_to<::core::settings::server_settings>(obj.at("server"))
          : ::core::settings::server_settings{};

  result.clients =
      (obj.if_contains("clients") != nullptr)
          ? value_to<std::vector<::core::settings::client_settings>>(
                obj.at("clients"))
          : std::vector<::core::settings::client_settings>{};
  return result;
}

/**
 * @brief `core::settings::communications` を JSON 値へ変換する。
 * @param json_value 変換先 JSON 値。JSON オブジェクトとして上書きされる。
 * @param communications 変換元の通信設定。
 * @details
 * - 出力先は JSON オブジェクトで上書きする。
 * - `server` と `clients` を対応する `tag_invoke` を使って JSON 値へ変換する。
 */
void tag_invoke([[maybe_unused]] value_from_tag tag, value &json_value,
                const ::core::settings::communications &communications) {
  object obj;
  obj["server"] = value_from(communications.server);
  obj["clients"] = value_from(communications.clients);
  json_value = std::move(obj);
}

} // namespace boost::json