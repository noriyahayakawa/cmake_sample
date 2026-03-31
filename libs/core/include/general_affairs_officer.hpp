/**
 * @file general_affairs_officer.hpp
 * @brief 生徒会役員の共通インターフェースを宣言する。
 */

#pragma once

namespace core {

/**
 * @brief 生徒会役員の基底クラス。
 * @details
 * 役員ごとの挨拶処理を `hello()` として統一し、派生クラスで必要に応じて
 * オーバーライドできる。
 */
class general_affairs_officer {
public:
  /** @brief 仮想デストラクタ。 */
  virtual ~general_affairs_officer() = default;

  /**
   * @brief 役員ごとの挨拶処理を実行する。
   * @details
   * 既定実装は何もしない。
   */
  virtual void hello() const;
};

} // namespace core