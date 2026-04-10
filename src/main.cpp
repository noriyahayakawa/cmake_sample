#include "main.hpp"

/**
 * @file main.cpp
 * @brief アプリケーションの起動処理、引数解析、ログ初期化を実装する。
 */

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace {

/**
 * @brief Boost.Log の初期設定を行う。
 * @param executable_path 実行ファイルパス（`argv[0]`）。
 * @details
 * - 共通属性を登録する。
 * - 標準エラー出力向けのコンソール sink を生成し、`info` 以上を出力する。
 * - 実行ファイルと同じディレクトリ配下に `logs/` を作成する。
 * - ファイルログは `logs/app_YYYYMMDD.log` 形式（時刻なし）で
 *   `trace` 以上を記録する。
 * - ローテーション済みファイルは `logs/app_YYYYMMDD_NNNNN.log` 形式で保存する。
 * - 当日分ファイルが既に存在する場合は追記モードで開く。
 * - 終了時の最終ローテーションを無効化し、通常終了では追記を継続する。
 * - 日付が変わったタイミング（毎日 00:00）で新しい日付ファイルへ切り替える。
 * - ファイルログには時刻、重大度、スレッドID、メッセージを出力する。
 * - グローバル属性として `ThreadID` を追加する。
 * @throws boost::filesystem::filesystem_error
 * ログディレクトリ作成に失敗した場合に送出される。
 */
static void init_logging(const char *executable_path) {
  const fs::path exe_dir =
      fs::absolute(fs::path(executable_path)).parent_path();
  const fs::path log_dir = exe_dir / "logs";
  const fs::path log_file_pattern = log_dir / "app_%Y%m%d.log";
  const fs::path rotated_file_pattern = log_dir / "app_%Y%m%d_%2N.log";

  boost::filesystem::create_directories(log_dir);

  logging::add_common_attributes();

  auto console_sink = logging::add_console_log(
      std::clog, keywords::format =
                     (expr::stream << "[" << logging::trivial::severity << "]"
                                   << ": " << expr::smessage));

  console_sink->set_filter(logging::trivial::severity >=
                           logging::trivial::info);

  auto file_sink = logging::add_file_log(
      keywords::file_name = log_file_pattern.string(),
      keywords::target_file_name = rotated_file_pattern.string(),
      keywords::open_mode = std::ios_base::app,
      keywords::enable_final_rotation = false,
      keywords::rotation_size = 256 * 1024 * 1024,
      keywords::time_based_rotation =
          sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::auto_flush = true,
      keywords::format =
          (expr::stream
           << expr::format_date_time<boost::posix_time::ptime>(
                  "TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
           //  << " <" << expr::attr<unsigned int>("LineID") << ">"
           << " [" << logging::trivial::severity << "]"
           << " ("
           << expr::attr<boost::log::attributes::current_thread_id::value_type>(
                  "ThreadID")
           << ")"
           << ": " << expr::smessage));

  file_sink->set_filter(logging::trivial::severity >= logging::trivial::trace);

  logging::core::get()->add_global_attribute(
      "ThreadID", logging::attributes::current_thread_id());
}

/**
 * @brief コマンドライン引数を解析して入力ファイルパスを決定する。
 * @param argc コマンドライン引数の個数。
 * @param argv コマンドライン引数配列。
 * @return 読み込み対象として確定した入力ファイルパス。
 * @throws core::exceptions::show_help
 * `-h` または `--help` が指定された場合に送出される。
 * @throws core::exceptions::my_error
 * 相対パス指定時に入力ファイルが見つからない場合、または状態取得に失敗した場合に送出される。
 * @details
 * - 位置引数 1 個目または `-i` / `--input` から入力ファイル名を取得する。
 * - 相対パスで見つからない場合は実行ファイル配置ディレクトリ基準でも探索する。
 * - 参照先が存在しても通常ファイルでない場合は失敗として扱う。
 * @note
 * 絶対パスが存在しない場合はこの関数では例外を送出せず、そのままパスを返す。
 */
const fs::path parse_args(int argc, char *argv[]) {
  fs::path exe_dir = fs::absolute(fs::path(argv[0])).parent_path();
  try {
    po::options_description desc("利用可能なオプション");
    desc.add_options()("input,i",
                       po::value<fs::path>()->default_value("app.jsonc"),
                       "入力ファイル")("help,h", "ヘルプを表示");

    auto help_text = [&desc]() {
      std::ostringstream oss;
      oss << desc;
      return oss.str();
    };

    po::variables_map vm;
    po::positional_options_description pos;
    pos.add("input", 1);
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(pos).run(),
        vm);

    if (vm.count("help")) {
      BOOST_THROW_EXCEPTION(core::exceptions::show_help{}
                            << core::exceptions::errinfo_message(help_text()));
    }

    po::notify(vm);
    fs::path input_file = vm["input"].as<fs::path>();

    boost::function<bool(const fs::path &)> is_regular_input_file =
        [](const fs::path &p) -> bool {
      if (fs::exists(p)) {
        boost::system::error_code ec;
        fs::file_status st = fs::status(p, ec);
        if (ec) {
          BOOST_THROW_EXCEPTION(boost::system::system_error(
              ec, "ファイル状態の取得に失敗しました。"));
        }
        return fs::is_regular_file(st);
      }
      return false;
    };

    if (!is_regular_input_file(input_file)) {
      if (input_file.is_relative()) {
        input_file = exe_dir / input_file;
        if (!is_regular_input_file(input_file)) {
          BOOST_THROW_EXCEPTION(
              core::exceptions::my_error{}
              << core::exceptions::errinfo_message(
                     "入力ファイルが存在しません。")
              << core::exceptions::errinfo_path(input_file.string()));
        }
      }
    }

    return input_file;
  } catch (const std::exception &e) {
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_message(
               "上位レベルのエラーが発生しました。")
        << boost::errinfo_nested_exception(std::current_exception()));
  }
}

} // namespace

/**
 * @brief アプリケーションのエントリポイント。
 * @param argc コマンドライン引数の個数。
 * @param argv コマンドライン引数の配列。
 * @details
 * - 起動直後にログ機能を初期化し、以後の例外情報をログ出力できる状態にする。
 * - Windows 環境ではコンソールのコードページを UTF-8
 * に設定し、終了時に復元する。
 * - コマンドライン引数を解析して入力ファイルパスを決定する。
 * - 入力ファイルを読み込み、共通設定を初期化する。
 * - `-h` / `--help` が指定された場合はヘルプを表示して正常終了する。
 * @return 正常終了時は 0、入力ファイル読み込み失敗時は 1、
 *         boost 例外時は 1、std 例外時は 2、不明な例外時は 3。
 */
int main(int argc, char *argv[]) {

#ifdef _WIN32
  const UINT oldOut = GetConsoleOutputCP();
  const UINT oldIn = GetConsoleCP();
  struct ConsoleCodePageGuard {
    UINT old_output_cp;
    UINT old_input_cp;

    ~ConsoleCodePageGuard() {
      SetConsoleOutputCP(old_output_cp);
      SetConsoleCP(old_input_cp);
    }
  };
  const ConsoleCodePageGuard code_page_guard{oldOut, oldIn};
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
int main(int argc, char *argv[]) {
#endif

  try {
    ::init_logging(argv[0]);
    auto &student_council = core::student_council::instance();
    student_council.hello();
    const auto input_file = ::parse_args(argc, argv);
    auto &options = core::settings::options::instance();
    options.read_input_file(input_file);
    student_council.run();
  } catch (const core::exceptions::show_help &e) {
    if (const std::string *msg =
            boost::get_error_info<core::exceptions::errinfo_message>(e)) {
      std::cout << *msg << std::endl;
    } else {
      std::cout << "Usage: app [Options]" << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "  -i [--input] arg    入力ファイル" << std::endl;
      std::cout << "  -h [--help]         ヘルプを表示" << std::endl;
    }
    return 0;
  } catch (const boost::exception &e) {
    BOOST_LOG_TRIVIAL(fatal) << boost::diagnostic_information(e) << std::endl;
    return 1;
  } catch (const std::exception &e) {
    BOOST_LOG_TRIVIAL(fatal) << e.what() << std::endl;
    return 2;
  } catch (...) {
    BOOST_LOG_TRIVIAL(fatal) << "不明なエラーが発生しました。" << std::endl;
    return 3;
  }

  return 0;
}
