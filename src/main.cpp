#include "main.hpp"

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
    std::cerr << boost::diagnostic_information(e) << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
  } catch (...) {
    std::cerr << "Unknown error occurred." << std::endl;
    return 3;
  }

  return 0;
}
