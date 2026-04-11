/**
 * @file test_settings.cpp
 * @brief 設定構造体の JSON 相互変換テストを実装する。
 */

#include "settings/commons.hpp"
#include "settings/communications.hpp"
#include "settings/input_file.hpp"

#include <boost/json.hpp>
#include <gtest/gtest.h>

// ─────────────────────────────────────────────
// commons
// ─────────────────────────────────────────────

/**
 * @brief JSON オブジェクトから `commons`
 * の各フィールドが読み取れることを確認する。
 */
TEST(commons_test, json_to_commons_reads_fields) {
  const auto jv = boost::json::parse(R"({"appName":"MyApp","version":"2.0"})");
  const auto c = boost::json::value_to<core::settings::commons>(jv);
  EXPECT_EQ(c.app_name, "MyApp");
  EXPECT_EQ(c.version, "2.0");
}

/**
 * @brief キーが存在しない JSON から `commons`
 * を生成すると空文字がデフォルト値になることを確認する。
 */
TEST(commons_test, json_to_commons_missing_keys_default_to_empty) {
  const auto jv = boost::json::parse("{}");
  const auto c = boost::json::value_to<core::settings::commons>(jv);
  EXPECT_EQ(c.app_name, "");
  EXPECT_EQ(c.version, "");
}

/**
 * @brief `commons` → JSON → `commons`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(commons_test, commons_to_json_roundtrip) {
  core::settings::commons src;
  src.app_name = "TestApp";
  src.version = "3.1";
  const auto jv = boost::json::value_from(src);
  const auto dst = boost::json::value_to<core::settings::commons>(jv);
  EXPECT_EQ(dst.app_name, src.app_name);
  EXPECT_EQ(dst.version, src.version);
}

/**
 * @brief フィールドがすべて空文字の `commons` を JSON
 * に変換するとキーが出力されないことを確認する。
 */
TEST(commons_test, commons_to_json_omits_empty_fields) {
  const core::settings::commons src{};
  const auto jv = boost::json::value_from(src);
  ASSERT_TRUE(jv.is_object());
  EXPECT_EQ(jv.as_object().size(), 0u);
}

// ─────────────────────────────────────────────
// server_settings
// ─────────────────────────────────────────────

/**
 * @brief JSON オブジェクトから `server_settings`
 * の各フィールドが読み取れることを確認する。
 */
TEST(server_settings_test, json_to_server_settings_reads_fields) {
  const auto jv = boost::json::parse(
      R"({"enable":true,"name":"main-server","service":"8080"})");
  const auto s = boost::json::value_to<core::settings::server_settings>(jv);
  EXPECT_TRUE(s.enable);
  EXPECT_EQ(s.name, "main-server");
  EXPECT_EQ(s.service, "8080");
}

/**
 * @brief キーが存在しない JSON から `server_settings`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(server_settings_test, json_to_server_settings_missing_keys_default) {
  const auto jv = boost::json::parse("{}");
  const auto s = boost::json::value_to<core::settings::server_settings>(jv);
  EXPECT_FALSE(s.enable);
  EXPECT_EQ(s.name, "");
  EXPECT_EQ(s.service, "");
}

/**
 * @brief `server_settings` → JSON → `server_settings`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(server_settings_test, server_settings_to_json_roundtrip) {
  core::settings::server_settings src;
  src.enable = true;
  src.name = "sv";
  src.service = "9000";
  const auto jv = boost::json::value_from(src);
  const auto dst = boost::json::value_to<core::settings::server_settings>(jv);
  EXPECT_EQ(dst.enable, src.enable);
  EXPECT_EQ(dst.name, src.name);
  EXPECT_EQ(dst.service, src.service);
}

// ─────────────────────────────────────────────
// client_settings
// ─────────────────────────────────────────────

/**
 * @brief JSON オブジェクトから `client_settings`
 * の各フィールドが読み取れることを確認する。
 */
TEST(client_settings_test, json_to_client_settings_reads_fields) {
  const auto jv = boost::json::parse(
      R"({"enable":true,"name":"c1","host":"127.0.0.1","service":"1234"})");
  const auto c = boost::json::value_to<core::settings::client_settings>(jv);
  EXPECT_TRUE(c.enable);
  EXPECT_EQ(c.name, "c1");
  EXPECT_EQ(c.host, "127.0.0.1");
  EXPECT_EQ(c.service, "1234");
}

/**
 * @brief キーが存在しない JSON から `client_settings`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(client_settings_test, json_to_client_settings_missing_keys_default) {
  const auto jv = boost::json::parse("{}");
  const auto c = boost::json::value_to<core::settings::client_settings>(jv);
  EXPECT_FALSE(c.enable);
  EXPECT_EQ(c.name, "");
  EXPECT_EQ(c.host, std::nullopt);
  EXPECT_EQ(c.service, std::nullopt);
}

/**
 * @brief `client_settings` → JSON → `client_settings`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(client_settings_test, client_settings_to_json_roundtrip) {
  core::settings::client_settings src;
  src.enable = true;
  src.name = "cli";
  src.host = "192.168.0.1";
  src.service = "5000";
  const auto jv = boost::json::value_from(src);
  const auto dst = boost::json::value_to<core::settings::client_settings>(jv);
  EXPECT_EQ(dst.enable, src.enable);
  EXPECT_EQ(dst.name, src.name);
  EXPECT_EQ(dst.host, src.host);
  EXPECT_EQ(dst.service, src.service);
}

// ─────────────────────────────────────────────
// communications
// ─────────────────────────────────────────────

/**
 * @brief JSON オブジェクトから `communications`
 * のサーバ設定とクライアント配列が読み取れることを確認する。
 */
TEST(communications_test, json_to_communications_reads_server_and_clients) {
  const auto jv = boost::json::parse(R"({
    "server": {"enable":true,"name":"sv","service":"8080"},
    "clients": [
      {"enable":true,"name":"c1","host":"localhost","service":"9000"}
    ]
  })");
  const auto comm = boost::json::value_to<core::settings::communications>(jv);
  EXPECT_TRUE(comm.server.enable);
  EXPECT_EQ(comm.server.name, "sv");
  ASSERT_EQ(comm.clients.size(), 1u);
  EXPECT_TRUE(comm.clients[0].enable);
  EXPECT_EQ(comm.clients[0].name, "c1");
  EXPECT_EQ(comm.clients[0].host, "localhost");
}

/**
 * @brief キーが存在しない JSON から `communications`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(communications_test, json_to_communications_missing_keys_default) {
  const auto jv = boost::json::parse("{}");
  const auto comm = boost::json::value_to<core::settings::communications>(jv);
  EXPECT_FALSE(comm.server.enable);
  EXPECT_TRUE(comm.clients.empty());
}

/**
 * @brief `communications` → JSON → `communications`
 * のラウンドトリップで値が保持されることを確認する。
 */
TEST(communications_test, communications_to_json_roundtrip) {
  core::settings::communications src;
  src.server.enable = true;
  src.server.name = "srv";
  src.server.service = "7777";
  core::settings::client_settings cl;
  cl.enable = false;
  cl.name = "cli";
  cl.host = "10.0.0.1";
  cl.service = "3000";
  src.clients.push_back(cl);
  const auto jv = boost::json::value_from(src);
  const auto dst = boost::json::value_to<core::settings::communications>(jv);
  EXPECT_EQ(dst.server.name, src.server.name);
  ASSERT_EQ(dst.clients.size(), 1u);
  EXPECT_EQ(dst.clients[0].host, src.clients[0].host);
}

// ─────────────────────────────────────────────
// input_file
// ─────────────────────────────────────────────

/**
 * @brief JSON から `input_file` の `commons`
 * セクションが読み取れることを確認する。
 */
TEST(input_file_test, json_to_input_file_reads_commons) {
  const auto jv =
      boost::json::parse(R"({"commons":{"appName":"App","version":"1.0"}})");
  const auto f = boost::json::value_to<core::settings::input_file>(jv);
  EXPECT_EQ(f.commons.app_name, "App");
  EXPECT_EQ(f.commons.version, "1.0");
}

/**
 * @brief JSON から `input_file` の `communications`
 * セクションが読み取れることを確認する。
 */
TEST(input_file_test, json_to_input_file_reads_communications) {
  const auto jv = boost::json::parse(R"({
    "commons": {"appName":"A","version":"1"},
    "communications": {
      "server": {"enable":true,"name":"s","service":"80"},
      "clients": [
        {"enable":true,"name":"c","host":"127.0.0.1","service":"9000"}
      ]
    }
  })");
  const auto f = boost::json::value_to<core::settings::input_file>(jv);
  EXPECT_EQ(f.commons.app_name, "A");
  EXPECT_TRUE(f.communications.server.enable);
  ASSERT_EQ(f.communications.clients.size(), 1u);
  EXPECT_EQ(f.communications.clients[0].host, "127.0.0.1");
}

/**
 * @brief キーが存在しない JSON から `input_file`
 * を生成するとデフォルト値になることを確認する。
 */
TEST(input_file_test, json_to_input_file_missing_keys_default) {
  const auto jv = boost::json::parse("{}");
  const auto f = boost::json::value_to<core::settings::input_file>(jv);
  EXPECT_EQ(f.commons.app_name, "");
  EXPECT_EQ(f.commons.version, "");
  EXPECT_FALSE(f.communications.server.enable);
  EXPECT_TRUE(f.communications.clients.empty());
}
