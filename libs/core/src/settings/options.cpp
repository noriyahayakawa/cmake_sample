#include "settings/options.hpp"
#include "exceptions/my_error.hpp"
#include "exceptions/show_help.hpp"
#include <array>
#include <fstream>
#include <stdexcept>

namespace core::settings {

namespace json = boost::json;

/**
 * @brief options のデフォルトコンストラクタ。
 * @details
 * コマンドラインオプション定義を初期化し、`--help` と `--input` を登録する。
 * `--input` の既定値は `input.jsonc`。
 */
options::options() : desc_("利用可能なオプション") {
  desc_.add_options()("help,h", "ヘルプを表示")(
      "input,i", po::value<fs::path>()->default_value("input.jsonc"),
      "入力ファイル");
}

/**
 * @brief コマンドライン引数を解析して入力ファイルパスを確定する。
 * @param argc 引数個数。
 * @param argv 引数配列。
 * @details
 * - 実行ファイルの絶対パスを保持する。
 * - 位置引数として入力ファイルを1件受け付ける。
 * - `--help` 指定時はヘルプメッセージを保持した `show_help` 例外を送出する。
 * - 入力ファイルが相対パスの場合は実行ファイル配置ディレクトリ基準で解決する。
 * - 指定ファイルが存在しない場合は `../etc` を探索し、見つからなければ
 * `my_error` を送出する。
 */
void options::store(int argc, char *argv[]) {

  executable_path_ = fs::absolute(argv[0]);

  po::positional_options_description pos;
  pos.add("input", 1);
  po::store(
      po::command_line_parser(argc, argv).options(desc_).positional(pos).run(),
      vm_);
  if (vm_.count("help")) {
    BOOST_THROW_EXCEPTION(core::exceptions::show_help{}
                          << core::exceptions::errinfo_message(help_text()));
  }
  po::notify(vm_);

  if (vm_.count("input")) {
    input_file_path_ = vm_["input"].as<fs::path>();
    input_file_path_.is_relative()
        ? input_file_path_ =
              executable_path_.parent_path() / input_file_path_.filename()
        : input_file_path_;
    boost::system::error_code ec;
    if (!is_regular_file_nothrow(input_file_path_, ec)) {
      input_file_path_ = input_file_path_.parent_path().parent_path() / "etc" /
                         input_file_path_.filename();
      if (!is_regular_file_nothrow(input_file_path_, ec)) {
        input_file_path_ =
            executable_path_.parent_path() / input_file_path_.filename();
        BOOST_THROW_EXCEPTION(
            core::exceptions::my_error{}
            << core::exceptions::errinfo_message(
                   "入力ファイルが存在しないか、通常のファイルではありません。")
            << core::exceptions::errinfo_path(input_file_path_.string())
            << core::exceptions::errinfo_errno(ec.value()));
      }
    }
  }
}

/**
 * @brief 入力ファイルを JSON として読み込み、共通設定を更新する。
 * @return 読み込み成功時は `true`。
 * @details
 * - バイナリモードでファイルを開く。
 * - ストリームパーサへ分割入力する。
 * - 先頭チャンクのみ UTF-8 BOM を検出した場合に除去する。
 * - `commons` キーが存在する場合、`commons_` を `value_to` で変換して更新する。
 * - 例外発生時は `my_error` に変換して再送出する。
 */
bool options::read_input_file() {
  try {
    std::ifstream ifs(input_file_path_.string(), std::ios::binary);
    if (!ifs) {
      throw boost::system::system_error(
          boost::system::error_code(errno, boost::system::generic_category()),
          "ファイルを開けませんでした");
    }

    json::stream_parser parser;
    boost::system::error_code ec;

    bool first_chunk = true;
    std::array<char, 4096> buffer{};
    while (ifs) {
      ifs.read(buffer.data(), buffer.size());

      std::streamsize read = ifs.gcount();
      if (read <= 0) {
        break;
      }

      const char *data = buffer.data();
      std::size_t size = static_cast<std::size_t>(read);

      if (first_chunk) {
        first_chunk = false;
        if (size >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
            static_cast<unsigned char>(data[1]) == 0xBB &&
            static_cast<unsigned char>(data[2]) == 0xBF) {
          data += 3;
          size -= 3;
        }
      }

      parser.write(data, size, ec);
      if (ec) {
        throw std::runtime_error("json parse error: " + ec.message());
      }
    }

    parser.finish(ec);
    if (ec) {
      throw std::runtime_error("json parse error: " + ec.message());
    }

    json::value jv = parser.release();
    const auto &obj = jv.as_object();
    if (obj.contains("commons")) {
      commons_ = boost::json::value_to<commons>(jv.at("commons"));
    }

    return true;
  } catch (const std::exception &e) {
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_message(e.what())
        << core::exceptions::errinfo_path(input_file_path_.string()));
    return false;
  }
}

/**
 * @brief オプション定義のヘルプ文字列を生成する。
 * @return `options_description` を整形した文字列。
 */
std::string options::help_text() {
  std::ostringstream oss;
  oss << desc_;
  return oss.str();
}

/**
 * @brief 例外を使わず通常ファイルかどうかを判定する。
 * @param p 判定対象パス。
 * @param ec 判定時のエラーコード出力。
 * @return 通常ファイルなら `true`、それ以外は `false`。
 */
bool options::is_regular_file_nothrow(const fs::path &p,
                                      boost::system::error_code &ec) {
  ec.clear();
  fs::file_status st = fs::status(p, ec);
  if (ec)
    return false;
  return fs::is_regular_file(st);
}

} // namespace core::settings