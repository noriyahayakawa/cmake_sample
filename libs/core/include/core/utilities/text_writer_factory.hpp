#pragma once

/**
 * @file text_writer_factory.hpp
 * @brief テキストファイル書き込みファクトリの宣言。
 */

#include "core_export.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/serialization/singleton.hpp>
#include <mutex>
#include <string>

namespace core::utilities {

namespace fs = boost::filesystem;
using boost::container::flat_map;

/**
 * @brief テキストファイルへのストリームを管理するシングルトンファクトリ。
 * @details
 * ファイル名をキーとしてファイルストリームをキャッシュし、同一名への
 * 重複オープンを避ける。コピーと代入は禁止されている。
 */
class CORE_EXPORT text_writer_factory : private boost::noncopyable {
public:
  /**
   * @brief `writer()` / `create_writer()` の戻り値型。
   */
  struct writer_result {
    fs::ofstream &writer; ///< 対象ファイルへの出力ストリーム参照。
    bool created;         ///< 今回新規作成されたファイルであれば `true`。
  };

private:
  friend class boost::serialization::singleton<text_writer_factory>;
  friend class boost::serialization::detail::singleton_wrapper<
      text_writer_factory>;
  std::mutex mutex_; ///< スレッドセーフなアクセスのためのミューテックス。

  /** @brief ファイル名からストリームオブジェクトへの対応表。 */
  flat_map<std::string, fs::ofstream> writers_;

  /** @brief 外部からのインスタンス生成を禁止するデフォルトコンストラクタ。 */
  text_writer_factory() = default;

  /**
   * @brief 新規ファイルを作成してストリームを追加する。
   * @param name ファイル名（出力ディレクトリに対する相対ファイル名）。
   * @return 作成したストリームへの参照と新規作成フラグ。
   * @throws core::exceptions::my_error ファイルのオープンに失敗した場合。
   * @details
   * 出力先パスは `core::settings::options` が保持する `output_dir` を
   * 基準に `name` を結合して確定する。
   * Windows 環境で新規作成した場合は UTF-8 BOM をファイル先頭に書き込む。
   */
  writer_result create_writer(const std::string &name);

public:
  /** @brief デストラクタ。 */
  ~text_writer_factory() = default;

  /**
   * @brief シングルトンインスタンスへの参照を返す。
   * @return `text_writer_factory` の唯一インスタンス。
   */
  static text_writer_factory &instance() {
    return boost::serialization::singleton<
        text_writer_factory>::get_mutable_instance();
  }

  /**
   * @brief 指定したファイル名のストリームを取得または作成して返す。
   * @param name ファイル名（出力ディレクトリに対する相対ファイル名）。
   * @return ストリームへの参照と新規作成フラグ。
   * @throws core::exceptions::my_error ファイルのオープンに失敗した場合。
   * @details
   * 既にキャッシュされているストリームが存在する場合はそれを返す。
   * 存在しない場合は `create_writer()` を呼んで新規作成する。
   */
  writer_result writer(const std::string &name);
};

} // namespace core::utilities