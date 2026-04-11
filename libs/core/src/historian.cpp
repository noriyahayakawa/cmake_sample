/**
 * @file historian.cpp
 * @brief historian の実装を定義する。
 */

#include "core/historian.hpp"
#include <boost/log/trivial.hpp>
#include <boost/version.hpp>

namespace core {

/**
 * @brief 記録係の挨拶処理。
 * @details
 * 現在の実装では Boost のバージョン情報をデバッグログへ出力する。
 */
void historian::hello() const {
  BOOST_LOG_TRIVIAL(debug) << "Boost version (string): " << BOOST_LIB_VERSION;
  BOOST_LOG_TRIVIAL(debug) << "Boost version (numeric): "
                           << BOOST_VERSION / 100000 << "."
                           << BOOST_VERSION / 100 % 1000 << "."
                           << BOOST_VERSION % 100;
}

} // namespace core