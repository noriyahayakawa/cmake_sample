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