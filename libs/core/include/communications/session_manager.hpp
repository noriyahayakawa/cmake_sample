#pragma once

/**
 * @file session_manager.hpp
 * @brief TCP セッション管理クラスを宣言する。
 */

#include <boost/asio.hpp>

namespace core::communications {

/**
 * @brief 複数の TCP セッションを管理するクラス。
 * @details
 * 接続セッションの生成・保持・破棄を一元管理する。
 * 現在の実装は空で、将来の拡張ポイントとして定義されている。
 */
class session_manager {};

} // namespace core::communications