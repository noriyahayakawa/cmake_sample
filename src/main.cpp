#include "main.hpp"

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

/**
 * @brief Boost.Log の初期設定を行う。
 * @details
 * - 共通属性を登録する。
 * - 標準エラー出力向けのコンソール sink を生成し、`info` 以上を出力する。
 * - ローテーション付きファイル sink を生成し、`trace` 以上を出力する。
 * -
 * ファイルログの出力フォーマットは時刻、行番号、重大度、スレッドID、メッセージを含む。
 * - グローバル属性として `ThreadID` を追加する。
 */
static void init_logging() {
  logging::add_common_attributes();

  auto console_sink = logging::add_console_log(
      std::clog, keywords::format =
                     (expr::stream << "[" << logging::trivial::severity << "]"
                                   << ": " << expr::smessage));

  console_sink->set_filter(logging::trivial::severity >=
                           logging::trivial::info);

  auto file_sink = logging::add_file_log(
      keywords::file_name = "logs/app_%Y%m%d_%H%M%S_%N.log",
      keywords::rotation_size = 10 * 1024 * 1024, keywords::auto_flush = true,
      keywords::format =
          (expr::stream
           << expr::format_date_time<boost::posix_time::ptime>(
                  "TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
           << " <" << expr::attr<unsigned int>("LineID") << ">"
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
 * @brief アプリケーションのエントリポイント。
 * @param argc コマンドライン引数の個数。
 * @param argv コマンドライン引数の配列。
 * @details
 * - Windows 環境ではコンソールのコードページを UTF-8
 * に設定し、終了時に復元する。
 * - コマンドライン引数を解析して入力ファイルパスを決定する。
 * - 入力ファイルを読み込む。
 * - `-h` / `--help` が指定された場合はヘルプを表示して正常終了する。
 * @return 正常終了時は 0、入力ファイル読み込み失敗時は 1、
 *         boost 例外時は 1、std 例外時は 2、不明な例外時は 3。
 */
int main(int argc, char *argv[]) {

#ifdef _WIN32
  const UINT oldOut = GetConsoleOutputCP();
  const UINT oldIn = GetConsoleCP();

  struct ConsoleCodePageRestorer {
    UINT outCP, inCP;
    ~ConsoleCodePageRestorer() {
      SetConsoleOutputCP(outCP);
      SetConsoleCP(inCP);
    }
  } cpRestorer{oldOut, oldIn};

  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif

  try {
    init_logging();
    core::settings::options::instance().store(argc, argv);
    if (!core::settings::options::instance().read_input_file()) {
      std::cerr << "Failed to read input file." << std::endl;
      return 1;
    }
  } catch (const core::exceptions::show_help &e) {
    if (const std::string *msg =
            boost::get_error_info<core::exceptions::errinfo_message>(e)) {
      std::cout << *msg << std::endl;
    } else {
      std::cout << "Usage: app [options]" << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "  -h [--help]         show help" << std::endl;
      std::cout << "  -i [--input] arg    input file" << std::endl;
    }
    return 0;
  } catch (const boost::exception &e) {
    BOOST_LOG_TRIVIAL(fatal) << boost::diagnostic_information(e) << std::endl;
    return 1;
  } catch (const std::exception &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Error: " << e.what() << std::endl;
    return 2;
  } catch (...) {
    BOOST_LOG_TRIVIAL(fatal) << "Unknown error occurred." << std::endl;
    return 3;
  }

  return 0;
}
