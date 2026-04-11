/**
 * @file vice_president.hpp
 * @brief 副会長クラスを宣言する。
 */

#pragma once

#include "core/general_affairs_officer.hpp"

namespace core {

/**
 * @brief 副会長を表すクラス。
 */
class CORE_EXPORT vice_president : public general_affairs_officer {
public:
  /** @brief デストラクタ。 */
  virtual ~vice_president() = default;

  /** @brief 副会長の挨拶処理を実行する。 */
  virtual void hello() const override;
};

} // namespace core