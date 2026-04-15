#include "settings/options.hpp"

/**
 * @file options.cpp
 * @brief 設定管理シングルトンクラスの読み込み処理を実装する。
 */

#include "exceptions/my_error.hpp"
#include "exceptions/show_help.hpp"
#include <array>
#include <boost/log/trivial.hpp>
#include <boost/system/system_error.hpp>
#include <boost/version.hpp>
#include <cctype>
#include <cerrno>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace core::settings {

namespace json = boost::json;

namespace {

constexpr std::array<unsigned char, 3> kUtf8Bom = {0xEFU, 0xBBU, 0xBFU};

/**
 * @brief 英字を小文字へ正規化した文字列を返す。
 * @param text 変換対象文字列。
 * @return 小文字化した文字列。
 */
auto to_lower_copy(std::string text) -> std::string {
  for (char &character : text) {
    character =
        static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
  }
  return text;
}

/**
 * @brief パスの拡張子が `.jsonc` かを判定する。
 * @param path 判定対象パス。
 * @retval true 拡張子が `.jsonc` である。
 * @retval false それ以外。
 */
auto is_jsonc_path(const fs::path &path) -> bool {
  return to_lower_copy(path.extension().string()) == ".jsonc";
}

} // namespace

/**
 * @brief 入力ファイルを JSON として読み込み、共通設定を更新する。
 * @param path 読み込む入力ファイルパス。
 * @details
 * - バイナリモードでファイルを開く。
 * - ファイル全体を文字列として読み込み、ストリームパーサへ入力する。
 * - 先頭に UTF-8 BOM がある場合は除去する。
 * - 拡張子が `.jsonc` の場合は行コメントと C 形式ブロックコメントを除去する。
 * - 解析結果は `input_file_` に格納し、`commons` キーが存在しない場合は
 *   既定構築した `commons` を保持する。
 * - 失敗時は発生した標準例外を `my_error` に包み直して再送出する。
 */
void options::read_input_file(const fs::path &path) {
  try {
    input_file_path_ = path;
    std::ifstream ifs(input_file_path_.string(),
                      std::ios::in | std::ios::binary);
    if (!ifs || !ifs.is_open()) {
      BOOST_THROW_EXCEPTION(boost::system::system_error(
          boost::system::error_code(errno, boost::system::generic_category()),
          "ファイルを開けませんでした"));
    }

    std::string input_text((std::istreambuf_iterator<char>(ifs)),
                           std::istreambuf_iterator<char>());

    if (input_text.size() >= kUtf8Bom.size() &&
        std::equal(kUtf8Bom.begin(), kUtf8Bom.end(), input_text.begin(),
                   [](unsigned char bom_byte, char input_char) -> bool {
                     return bom_byte == static_cast<unsigned char>(input_char);
                   })) {
      input_text.erase(0, kUtf8Bom.size());
    }

    json::parse_options opt;
    opt.allow_comments = is_jsonc_path(input_file_path_);
    opt.allow_trailing_commas = true;
    json::value json_value = json::parse(input_text, {}, opt);
    input_file_ = json::value_to<settings::input_file>(json_value);
    input_file_.resolve_relative_path(input_file_path_.parent_path());

    BOOST_LOG_TRIVIAL(debug)
        << "入力ファイルを読み込みました。: "
        << boost::json::serialize(boost::json::value_from(input_file_));

  } catch (const std::exception &e) {
    BOOST_THROW_EXCEPTION(core::exceptions::my_error{}
                          << core::exceptions::errinfo_message(e.what())
                          << core::exceptions::errinfo_path(path.string()));
  } catch (...) {
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_message("不明な例外が発生しました")
        << core::exceptions::errinfo_path(path.string()));
  }
}

} // namespace core::settings