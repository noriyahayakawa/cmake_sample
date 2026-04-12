#pragma once

/**
 * @file message.hpp
 * @brief TCP メッセージの基底クラスを宣言する。
 */

#include "comm_export.hpp"
#include <string>

namespace comm::messages {

/**
 * @brief TCP メッセージの基底構造体。
 *
 * すべてのメッセージ型はこの構造体を継承して定義する。
 * `message_factory` による生成・管理のために仮想デストラクタを持つ。
 */
struct COMM_EXPORT message {
  /** @brief デフォルトコンストラクタ。 */
  message() = default;

  /** @brief 仮想デストラクタ。 */
  virtual ~message() = default;

  /**
   * @brief メッセージをバイト列にエンコードする。
   * @return エンコードされたデータを表す文字列。
   */
  virtual std::string encode() const = 0;

  /**
   * @brief バイト列からメッセージをデコードする。
   * @param data デコード対象のデータを表す文字列。
   */
  virtual void decode(const std::string &data) = 0;
};

} // namespace comm::messages