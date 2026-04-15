/**
 * @file test_settings.cpp
 * @brief 設定構造体の JSON 相互変換テストを実装する。
 */

#include "core/settings/commons.hpp"
#include "core/settings/communications.hpp"
#include "core/settings/input_file.hpp"

#include <boost/json.hpp>
#include <gtest/gtest.h>

/**
 * @brief JSON オブジェクトから `commons`
 * の各フィールドが読み取れることを確認する。
 */
TEST(commons_test, json_to_commons_reads_fields) {
  const auto json_value =
      boost::json::parse(R"({"appName":"MyApp","version":"2.0"})");
  const auto commons =
      boost::json::value_to<core::settings::commons>(json_value);
  EXPECT_EQ(commons.app_name, "MyApp");
  EXPECT_EQ(commons.version, "2.0");
}

/**
 * @brief キーが存在しない JSON から `commons`
 * を生成すると空文字がデフォルト値になることを確認する。
 */
TEST(commons_test, json_to_commons_missing_keys_default_to_empty) {
  const auto json_value = boost::json::parse("{}");
  const auto commons =
      boost::json::value_to<core::settings::commons>(json_value);
  EXPECT_EQ(commons.app_name, "");
  EXPECT_EQ(commons.version, "");
}

/**
 * @brief `commons` → JSON → `commons`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(commons_test, commons_to_json_roundtrip) {
  core::settings::commons src;
  src.app_name = "TestApp";
  src.version = "3.1";
  const auto json_value = boost::json::value_from(src);
  const auto dst = boost::json::value_to<core::settings::commons>(json_value);
  EXPECT_EQ(dst.app_name, src.app_name);
  EXPECT_EQ(dst.version, src.version);
}

/**
 * @brief フィールドがすべて空文字の `commons` を JSON
 * に変換するとキーが出力されないことを確認する。
 */
TEST(commons_test, commons_to_json_omits_empty_fields) {
  const core::settings::commons src{};
  const auto json_value = boost::json::value_from(src);
  ASSERT_TRUE(json_value.is_object());
  EXPECT_EQ(json_value.as_object().size(), 1U);
  EXPECT_TRUE(json_value.as_object().if_contains("output_dir") != nullptr);
}

/**
 * @brief JSON オブジェクトから `server_settings`
 * の各フィールドが読み取れることを確認する。
 */
TEST(server_settings_test, json_to_server_settings_reads_fields) {
  constexpr char kServicePort[] = "8080";
  const auto json_value = boost::json::parse(
      R"({"enable":true,"id":"main-server","service":"8080"})");
  const auto server_settings =
      boost::json::value_to<core::settings::server_settings>(json_value);
  EXPECT_TRUE(server_settings.enable);
  EXPECT_EQ(server_settings.id, "main-server");
  EXPECT_EQ(server_settings.service, kServicePort);
}

/**
 * @brief キーが存在しない JSON から `server_settings`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(server_settings_test, json_to_server_settings_missing_keys_default) {
  const auto json_value = boost::json::parse("{}");
  const auto server_settings =
      boost::json::value_to<core::settings::server_settings>(json_value);
  EXPECT_FALSE(server_settings.enable);
  EXPECT_EQ(server_settings.id, "");
  EXPECT_EQ(server_settings.service, "");
}

/**
 * @brief `server_settings` → JSON → `server_settings`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(server_settings_test, server_settings_to_json_roundtrip) {
  core::settings::server_settings server_src;
  server_src.enable = true;
  server_src.id = "sv";
  server_src.service = "9000";
  const auto json_value = boost::json::value_from(server_src);
  const auto server_dst =
      boost::json::value_to<core::settings::server_settings>(json_value);
  EXPECT_EQ(server_dst.enable, server_src.enable);
  EXPECT_EQ(server_dst.id, server_src.id);
  EXPECT_EQ(server_dst.service, server_src.service);
}

/**
 * @brief JSON オブジェクトから `client_settings`
 * の各フィールドが読み取れることを確認する。
 */
TEST(client_settings_test, json_to_client_settings_reads_fields) {
  constexpr char kClientService[] = "1234";
  const auto json_value = boost::json::parse(
      R"({"enable":true,"id":"c1","host":"127.0.0.1","service":"1234"})");
  const auto client_settings =
      boost::json::value_to<core::settings::client_settings>(json_value);
  EXPECT_TRUE(client_settings.enable);
  EXPECT_EQ(client_settings.id, "c1");
  EXPECT_EQ(client_settings.host, "127.0.0.1");
  EXPECT_EQ(client_settings.service, kClientService);
}

/**
 * @brief キーが存在しない JSON から `client_settings`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(client_settings_test, json_to_client_settings_missing_keys_default) {
  const auto json_value = boost::json::parse("{}");
  const auto client_settings =
      boost::json::value_to<core::settings::client_settings>(json_value);
  EXPECT_FALSE(client_settings.enable);
  EXPECT_EQ(client_settings.id, "");
  EXPECT_EQ(client_settings.host, "");
  EXPECT_EQ(client_settings.service, "");
}

/**
 * @brief `client_settings` → JSON → `client_settings`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(client_settings_test, client_settings_to_json_roundtrip) {
  core::settings::client_settings client_src;
  client_src.enable = true;
  client_src.id = "cli";
  client_src.host = "192.168.0.1";
  client_src.service = "5000";
  const auto json_value = boost::json::value_from(client_src);
  const auto client_dst =
      boost::json::value_to<core::settings::client_settings>(json_value);
  EXPECT_EQ(client_dst.enable, client_src.enable);
  EXPECT_EQ(client_dst.id, client_src.id);
  EXPECT_EQ(client_dst.host, client_src.host);
  EXPECT_EQ(client_dst.service, client_src.service);
}

/**
 * @brief JSON オブジェクトから `communications`
 * のサーバ設定とクライアント配列が読み取れることを確認する。
 */
TEST(communications_test, json_to_communications_reads_server_and_clients) {
  constexpr char kServerPort[] = "8080";
  constexpr char kClientPort[] = "9000";
  const auto json_value = boost::json::parse(R"({
      "server": {"enable":true,"id":"sv","service":"8080"},
      "clients": [
        {"enable":true,"id":"c1","host":"localhost","service":"9000"}
      ]
    })");
  const auto comm =
      boost::json::value_to<core::settings::communications>(json_value);
  EXPECT_TRUE(comm.server.enable);
  EXPECT_EQ(comm.server.id, "sv");
  ASSERT_EQ(comm.clients.size(), 1U);
  EXPECT_TRUE(comm.clients[0].enable);
  EXPECT_EQ(comm.clients[0].id, "c1");
  EXPECT_EQ(comm.clients[0].host, "localhost");
  EXPECT_EQ(comm.server.service, kServerPort);
  EXPECT_EQ(comm.clients[0].service, kClientPort);
}

/**
 * @brief キーが存在しない JSON から `communications`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(communications_test, json_to_communications_missing_keys_default) {
  const auto json_value = boost::json::parse("{}");
  const auto comm =
      boost::json::value_to<core::settings::communications>(json_value);
  EXPECT_FALSE(comm.server.enable);
  EXPECT_TRUE(comm.clients.empty());
}

/**
 * @brief `communications` → JSON → `communications`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(communications_test, communications_to_json_roundtrip) {
  core::settings::communications comm_src;
  comm_src.server.enable = true;
  comm_src.server.id = "srv";
  constexpr char kCommServerPort[] = "7777";
  comm_src.server.service = kCommServerPort;
  core::settings::client_settings comm_client_settings;
  comm_client_settings.enable = false;
  comm_client_settings.id = "cli";
  comm_client_settings.host = "10.0.0.1";
  constexpr char kCommClientPort[] = "3000";
  comm_client_settings.service = kCommClientPort;
  comm_src.clients.push_back(comm_client_settings);
  const auto json_value = boost::json::value_from(comm_src);
  const auto comm_dst =
      boost::json::value_to<core::settings::communications>(json_value);
  EXPECT_EQ(comm_dst.server.id, comm_src.server.id);
  ASSERT_EQ(comm_dst.clients.size(), 1U);
  EXPECT_EQ(comm_dst.clients[0].host, comm_src.clients[0].host);
}

/**
 * @brief JSON から `input_file` の `commons`
 * セクションが読み取れることを確認する。
 */
TEST(input_file_test, json_to_input_file_reads_commons) {
  constexpr char kVersion[] = "1.0";
  const auto json_value =
      boost::json::parse(R"({"commons":{"appName":"App","version":"1.0"}})");
  const auto input_file =
      boost::json::value_to<core::settings::input_file>(json_value);
  EXPECT_EQ(input_file.commons.app_name, "App");
  EXPECT_EQ(input_file.commons.version, kVersion);
}

/**
 * @brief JSON から `input_file` の `communications`
 * セクションが読み取れることを確認する。
 */
TEST(input_file_test, json_to_input_file_reads_communications) {
  constexpr char kInputVersion[] = "1";
  constexpr char kInputServerPort[] = "80";
  constexpr char kInputClientPort[] = "9000";
  const auto json_value = boost::json::parse(R"({
      "commons": {"appName":"A","version":"1"},
      "communications": {
        "server": {"enable":true,"id":"s","service":"80"},
        "clients": [
          {"enable":true,"id":"c","host":"127.0.0.1","service":"9000"}
        ]
      }
    })");
  const auto input_file =
      boost::json::value_to<core::settings::input_file>(json_value);
  EXPECT_EQ(input_file.commons.app_name, "A");
  EXPECT_EQ(input_file.commons.version, kInputVersion);
  EXPECT_TRUE(input_file.communications.server.enable);
  ASSERT_EQ(input_file.communications.clients.size(), 1U);
  EXPECT_EQ(input_file.communications.server.service, kInputServerPort);
  EXPECT_EQ(input_file.communications.clients[0].host, "127.0.0.1");
  EXPECT_EQ(input_file.communications.clients[0].service, kInputClientPort);
}

/**
 * @brief キーが存在しない JSON から `input_file`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(input_file_test, json_to_input_file_missing_keys_default) {
  const auto json_value = boost::json::parse("{}");
  const auto input_file =
      boost::json::value_to<core::settings::input_file>(json_value);
  EXPECT_EQ(input_file.commons.app_name, "");
  EXPECT_EQ(input_file.commons.version, "");
  EXPECT_FALSE(input_file.communications.server.enable);
  EXPECT_TRUE(input_file.communications.clients.empty());
}
