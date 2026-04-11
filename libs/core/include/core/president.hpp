/**
 * @file president.hpp
 * @brief 生徒会長クラスを宣言する。
 */

#pragma once

#include "core/general_affairs_officer.hpp"

namespace core {

/**
 * @brief 生徒会長を表すクラス。
 */
class CORE_EXPORT president : public general_affairs_officer {
public:
  /** @brief デストラクタ。 */
  virtual ~president() = default;

  /** @brief 生徒会長の挨拶処理を実行する。 */
  virtual void hello() const override;
};

} // namespace core