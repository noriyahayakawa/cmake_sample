#include "exceptions/my_error.hpp"
#include "settings/options.hpp"

/**
 * @file test_main.cpp
 * @brief 設定読み込み処理の単体テストを実装する。
 */

#include <fstream>
#include <gtest/gtest.h>

/**
 * @brief 存在しない入力ファイルの読み込みが `my_error`
 * を送出することを確認する。
 */
TEST(options_test, read_input_file_with_missing_file_throws_my_error) {
  const boost::filesystem::path missing_file =
      "this-file-should-not-exist.json";

  EXPECT_THROW(
      core::settings::options::instance().read_input_file(missing_file),
      core::exceptions::my_error);
}

/**
 * @brief `.jsonc` 入力ではコメントを無視して読み込めることを確認する。
 */
TEST(options_test, read_jsonc_file_ignores_comments) {
  const boost::filesystem::path temp_path =
      boost::filesystem::temp_directory_path() /
      boost::filesystem::unique_path("settings-%%%%-%%%%.jsonc");

  {
    std::ofstream ofs(temp_path.string(), std::ios::binary);
    ASSERT_TRUE(ofs.is_open());
    ofs << "{\n"
           "  // line comment\n"
           "  \"commons\": {\n"
           "    \"appName\": \"sample\", /* block comment */\n"
           "    \"version\": \"1.0\"\n"
           "  }\n"
           "}\n";
  }

  EXPECT_NO_THROW(
      core::settings::options::instance().read_input_file(temp_path));

  boost::system::error_code ec;
  boost::filesystem::remove(temp_path, ec);
}

/**
 * @brief 通常の `.json` ファイルが正常に読み込めることを確認する。
 */
TEST(options_test, read_json_file_succeeds) {
  const boost::filesystem::path temp_path =
      boost::filesystem::temp_directory_path() /
      boost::filesystem::unique_path("settings-%%%%-%%%%.json");

  {
    std::ofstream ofs(temp_path.string(), std::ios::binary);
    ASSERT_TRUE(ofs.is_open());
    ofs << R"({"commons":{"appName":"test","version":"0.1"}})";
  }

  EXPECT_NO_THROW(
      core::settings::options::instance().read_input_file(temp_path));

  boost::system::error_code ec;
  boost::filesystem::remove(temp_path, ec);
}

/**
 * @brief `read_input_file` 後に `input_file()`
 * で読み込み内容が取得できることを確認する。
 */
TEST(options_test, input_file_reflects_loaded_content) {
  const boost::filesystem::path temp_path =
      boost::filesystem::temp_directory_path() /
      boost::filesystem::unique_path("settings-%%%%-%%%%.json");

  constexpr char kHelloVersion[] = "9.9";

  {
    std::ofstream ofs(temp_path.string(), std::ios::binary);
    ASSERT_TRUE(ofs.is_open());
    ofs << R"({"commons":{"appName":"hello","version":"9.9"}})";
  }

  core::settings::options::instance().read_input_file(temp_path);
  const auto &input_file = core::settings::options::instance().input_file();
  EXPECT_EQ(input_file.commons.app_name, "hello");
  EXPECT_EQ(input_file.commons.version, kHelloVersion);

  boost::system::error_code ec;
  boost::filesystem::remove(temp_path, ec);
}

/**
 * @brief UTF-8 BOM 付きファイルでも正常に読み込めることを確認する。
 */
TEST(options_test, read_json_file_with_utf8_bom_succeeds) {
  const boost::filesystem::path temp_path =
      boost::filesystem::temp_directory_path() /
      boost::filesystem::unique_path("settings-%%%%-%%%%.json");

  {
    std::ofstream ofs(temp_path.string(), std::ios::binary);
    ASSERT_TRUE(ofs.is_open());
    ofs.write("\xEF\xBB\xBF", 3);
    ofs << R"({"commons":{"appName":"bom","version":"1"}})";
  }

  EXPECT_NO_THROW(
      core::settings::options::instance().read_input_file(temp_path));

  boost::system::error_code ec;
  boost::filesystem::remove(temp_path, ec);
}

/**
 * @brief `communications` セクション付きの JSON
 * ファイルが正しく読み込めることを確認する。
 */
TEST(options_test, read_json_with_communications_section) {
  const boost::filesystem::path temp_path =
      boost::filesystem::temp_directory_path() /
      boost::filesystem::unique_path("settings-%%%%-%%%%.json");

  {
    std::ofstream ofs(temp_path.string(), std::ios::binary);
    ASSERT_TRUE(ofs.is_open());
    ofs << R"({
      "commons": {"appName":"app","version":"1.0"},
      "communications": {
        "server": {"enable":true,"id":"srv","service":"8080"},
        "clients": [
          {"enable":true,"id":"c1","host":"127.0.0.1","service":"9000"}
        ]
      }
    })";
  }

  core::settings::options::instance().read_input_file(temp_path);
  const auto &input_file = core::settings::options::instance().input_file();
  EXPECT_TRUE(input_file.communications.server.enable);
  EXPECT_EQ(input_file.communications.server.id, "srv");
  ASSERT_EQ(input_file.communications.clients.size(), 1u);
  EXPECT_EQ(input_file.communications.clients[0].host, "127.0.0.1");

  boost::system::error_code ec;
  boost::filesystem::remove(temp_path, ec);
}