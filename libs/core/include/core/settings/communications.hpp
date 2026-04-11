#pragma once

/**
 * @file communications.hpp
 * @brief 通信設定構造体と JSON 変換関数を宣言する。
 */

#include "core/settings/i_input_data.hpp"
#include "core_export.hpp"
#include <boost/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace core::settings {

/**
 * @brief サーバ接続設定を保持する構造体。
 * @details
 * 入力 JSON の `server` オブジェクトに対応する値オブジェクト。
 * `boost::json` の `tag_invoke` カスタマイゼーションポイントにより JSON 値との
 * 相互変換ができる。
 */
struct CORE_EXPORT server_settings : public i_input_data {
  /** @brief 仮想デストラクタ。 */
  virtual ~server_settings() override = default;

  /** @brief サーバを有効にするかどうか。 */
  bool enable = false;
  /** @brief サーバの識別名。 */
  std::string name;
  /** @brief サーバのサービス名またはポート番号文字列。 */
  std::optional<std::string> service;

  virtual void
  resolve_relative_path(const boost::filesystem::path &path) override;
};

/**
 * @brief クライアント接続設定を保持する構造体。
 * @details
 * 入力 JSON の `clients` 配列の各要素に対応する値オブジェクト。
 * `boost::json` の `tag_invoke` カスタマイゼーションポイントにより JSON 値との
 * 相互変換ができる。
 */
struct CORE_EXPORT client_settings : public i_input_data {
  /** @brief 仮想デストラクタ。 */
  virtual ~client_settings() override = default;

  /** @brief クライアントを有効にするかどうか。 */
  bool enable = false;
  /** @brief クライアントの識別名。 */
  std::string name;
  /** @brief 接続先アドレス。 */
  std::optional<std::string> host;
  /** @brief 接続先サービス名またはポート番号文字列。 */
  std::optional<std::string> service;

  virtual void
  resolve_relative_path(const boost::filesystem::path &path) override;
};

/**
 * @brief 通信全体の設定を保持する構造体。
 * @details
 * 入力 JSON の `communications` オブジェクトに対応する値オブジェクト。
 * サーバ設定 1 件とクライアント設定の配列を保持する。
 * `boost::json` の `tag_invoke` カスタマイゼーションポイントにより JSON 値との
 * 相互変換ができる。
 */
struct CORE_EXPORT communications : public i_input_data {
  /** @brief 仮想デストラクタ。 */
  virtual ~communications() override = default;

  /** @brief サーバ接続設定。 */
  server_settings server;
  /** @brief クライアント接続設定の一覧。 */
  std::vector<client_settings> clients;

  virtual void
  resolve_relative_path(const boost::filesystem::path &path) override;
};

} // namespace core::settings

namespace boost::json {

/**
 * @brief JSON 値を `core::settings::server_settings` に変換する。
 * @param jv 変換元の JSON 値。
 * @return 変換結果の `server_settings` 構造体。
 * @details 各キーが存在しない場合はメンバの既定値（`false`
 * または空文字）を設定する。
 */
CORE_EXPORT ::core::settings::server_settings
tag_invoke(value_to_tag<::core::settings::server_settings>, const value &jv);

/**
 * @brief `core::settings::server_settings` を JSON 値に変換する。
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param server_settings 変換元のサーバ設定。
 * @details `enable`、`name`、`service` をすべて JSON オブジェクトへ格納する。
 */
CORE_EXPORT void
tag_invoke(value_from_tag, value &jv,
           const ::core::settings::server_settings &server_settings);

/**
 * @brief JSON 値を `core::settings::client_settings` に変換する。
 * @param jv 変換元の JSON 値。
 * @return 変換結果の `client_settings` 構造体。
 * @details 各キーが存在しない場合はメンバの既定値（`false`
 * または空文字）を設定する。
 */
CORE_EXPORT ::core::settings::client_settings
tag_invoke(value_to_tag<::core::settings::client_settings>, const value &jv);

/**
 * @brief `core::settings::client_settings` を JSON 値に変換する。
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param client_settings 変換元のクライアント設定。
 * @details `enable`、`name`、`host`、`service` をすべて JSON
 * オブジェクトへ格納する。
 */
CORE_EXPORT void
tag_invoke(value_from_tag, value &jv,
           const ::core::settings::client_settings &client_settings);

/**
 * @brief JSON 値を `core::settings::communications` に変換する。
 * @param jv 変換元の JSON 値。
 * @return 変換結果の `communications` 構造体。
 * @details
 * `server` キーが存在しない場合は既定構築した `server_settings` を設定する。
 * `clients` キーが存在しない場合は空の配列を設定する。
 */
CORE_EXPORT ::core::settings::communications
tag_invoke(value_to_tag<::core::settings::communications>, const value &jv);

/**
 * @brief `core::settings::communications` を JSON 値に変換する。
 * @param jv 変換先の JSON 値。JSON オブジェクトとして上書きされる。
 * @param communications 変換元の通信設定。
 * @details `server` と `clients` を JSON オブジェクトへ格納する。
 */
CORE_EXPORT void
tag_invoke(value_from_tag, value &jv,
           const ::core::settings::communications &communications);

} // namespace boost::json