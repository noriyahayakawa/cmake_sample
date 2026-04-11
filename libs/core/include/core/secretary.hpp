/**
 * @file secretary.hpp
 * @brief 書記クラスを宣言する。
 */

#pragma once

#include "core/general_affairs_officer.hpp"

namespace core {

/**
 * @brief 書記を表すクラス。
 */
class CORE_EXPORT secretary : public general_affairs_officer {
public:
  /** @brief デストラクタ。 */
  virtual ~secretary() = default;

  /** @brief 書記の挨拶処理を実行する。 */
  virtual void hello() const override;
};

} // namespace core