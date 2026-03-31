/**
 * @file treasurer.hpp
 * @brief 会計クラスを宣言する。
 */

#pragma once

#include "general_affairs_officer.hpp"

namespace core {

/**
 * @brief 会計を表すクラス。
 */
class treasurer : public general_affairs_officer {
public:
  /** @brief デストラクタ。 */
  virtual ~treasurer() = default;

  /** @brief 会計の挨拶処理を実行する。 */
  virtual void hello() const override;
};

} // namespace core