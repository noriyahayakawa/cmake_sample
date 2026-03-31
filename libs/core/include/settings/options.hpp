#pragma once

/**
 * @file options.hpp
 * @brief 設定管理シングルトンクラスを宣言する。
 */

#include "settings/input_file.hpp"
#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/singleton.hpp>
#include <boost/system/error_code.hpp>

namespace core::settings {

namespace fs = boost::filesystem;

/**
 * @brief アプリケーション全体の設定を管理するシングルトンクラス。
 * @details
 * `boost::serialization::singleton` を利用してプロセス内で単一インスタンスを
 * 提供する。
 * 入力ファイルのパスと、そこから読み込んだ共通設定を保持する。
 * コピーと代入は禁止されている。
 */
class options : private boost::noncopyable {
private:
  friend class boost::serialization::singleton<options>;
  friend class boost::serialization::detail::singleton_wrapper<options>;

  /** @brief 確定した入力ファイルパス。 */
  fs::path input_file_path_;

  /** @brief JSON ファイルから読み込まれた入力ファイル全体。 */
  input_file input_file_;

  /** @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。 */
  options() = default;

public:
  /** @brief デストラクタ。 */
  virtual ~options() = default;

  /**
   * @brief options の唯一のインスタンスへの参照を返す。
   * @return シングルトンインスタンスへの参照。
   */
  static options &instance() {
    return boost::serialization::singleton<options>::get_mutable_instance();
  }

  /**
   * @brief 確定した入力ファイルを読み込み、共通設定を内部に格納する。
   * @param path 読み込む JSON 形式の入力ファイルパス。
   * @throws core::exceptions::my_error
   * ファイルのオープン失敗や JSON 解析失敗時に送出される。
   * @details
   * ファイルはバイナリモードで開かれ、UTF-8 BOM があれば先頭から除去したうえで
   * `boost::json::stream_parser` に渡される。
   * 拡張子が `.jsonc` の場合は行コメント（`//`）および
   * C 形式ブロックコメントを除去してから解析する。
   * 解析結果は `input_file_` に格納され、`commons` キーが存在しない場合は
   * 既定構築した `commons` を保持する。
   */
  void read_input_file(const fs::path path);

  /**
   * @brief 最後に読み込んだ入力ファイル内容を返す。
   * @return 保持中の入力ファイル構造体への const 参照。
   */
  const input_file &input_file() const { return input_file_; }
};

} // namespace core::settings
