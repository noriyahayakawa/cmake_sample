#pragma once

#include "settings/commons.hpp"
#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/system/error_code.hpp>

namespace core::settings {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

/**
 * @brief アプリケーション全体の設定を管理するシングルトンクラス。
 *
 * boost::serialization::singleton を利用してシングルトンパターンを実現する。
 * コピー・代入は禁止されている。
 */
class options : private boost::noncopyable {
private:
  friend class boost::serialization::singleton<options>;
  friend class boost::serialization::detail::singleton_wrapper<options>;

  /** @brief コマンドラインオプション定義。 */
  po::options_description desc_;

  /** @brief 解析済みオプション値。 */
  po::variables_map vm_;

  /** @brief 確定した入力ファイルパス。 */
  fs::path input_file_path_;

  /** @brief 実行ファイルの絶対パス（argv[0] から取得）。 */
  fs::path executable_path_;

  /** @brief JSON ファイルから読み込まれたアプリケーション共通設定。 */
  commons commons_;

  /** @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。 */
  options();

  /**
   * @brief 指定パスが通常ファイルかを例外を使わずに判定する。
   * @param p 判定対象パス。
   * @param ec エラーコード（判定前にクリアされ、失敗時に設定される）。
   * @return 通常ファイルなら true、それ以外は false。
   */
  static bool is_regular_file_nothrow(const fs::path &p,
                                      boost::system::error_code &ec);

public:
  /** @brief デストラクタ。 */
  virtual ~options() = default;

  /**
   * @brief options の唯一のインスタンスへの参照を返す。
   * @return options& シングルトンインスタンスへの参照。
   */
  static options &instance() {
    return boost::serialization::singleton<options>::get_mutable_instance();
  }

  /**
   * @brief コマンドライン引数を解析し、入力ファイルパスを確定する。
   * @param argc 引数個数。
   * @param argv 引数配列。
   * @details
   * - `-h` または `--help` 指定時はヘルプ情報付き例外を送出する。
   * - `-i` 未指定時は `input.jsonc` を既定値として扱う。
   * - 相対パスは実行ファイル位置を基準に解決し、見つからない場合は
   *   実行ファイルから見た `../etc` も探索する。
   * @throw core::exceptions::show_help ヘルプ表示要求時。
   * @throw core::exceptions::my_error 入力ファイルが有効でない場合。
   */
  void store(int argc, char *argv[]);

  /**
   * @brief 確定した入力ファイルを読み込み、共通設定を内部に格納する。
   * @return 読み込みに成功した場合は true、失敗した場合は false。
   */
  bool read_input_file();

  /**
   * @brief ヘルプ表示用のオプション説明文字列を返す。
   * @return オプション説明文字列。
   */
  std::string help_text();
};

} // namespace core::settings
