#pragma once

/**
 * @file session.hpp
 * @brief TCP セッションの基底クラスを宣言する。
 */

#include <boost/asio.hpp>

namespace core::communications {

/**
 * @brief TCP セッションを表す基底クラス。
 * @details
 * 個別の接続ごとのセッション管理を担う基底型。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class session {};

} // namespace core::communications