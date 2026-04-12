/**
 * @file historian.hpp
 * @brief 記録係クラスを宣言する。
 */

#pragma once

#include "core/general_affairs_officer.hpp"
#include "core_export.hpp"

namespace core {

/**
 * @brief 記録係を表すクラス。
 * @details
 * 現在の実装では `hello()` で Boost バージョン情報をログ出力する。
 */
class CORE_EXPORT historian : public general_affairs_officer {
public:
  /** @brief デストラクタ。 */
  virtual ~historian() = default;

  /** @brief 記録係の挨拶処理を実行する。 */
  virtual void hello() const override;
};

} // namespace core