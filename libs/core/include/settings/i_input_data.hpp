/**
 * @file i_input_data.hpp
 * @brief 入力データの共通インターフェースを宣言する。
 */

#pragma once

#include <boost/filesystem.hpp>

namespace core::settings {

/**
 * @brief 入力データ共通のパス解決インターフェース。
 */
struct i_input_data {
  /** @brief 仮想デストラクタ。 */
  virtual ~i_input_data() = default;

  /**
   * @brief 相対パス項目を基準パスから絶対化する。
   * @param path 相対パス解決の基準となるディレクトリパス。
   */
  virtual void resolve_relative_path(const boost::filesystem::path &path) = 0;
};

} // namespace core::settings