/**
 * @file historian.cpp
 * @brief historian の実装を定義する。
 */

#include "historian.hpp"
#include <boost/log/trivial.hpp>
#include <boost/version.hpp>

namespace core {

namespace {

constexpr auto kBoostMajorDivisor = 100000;
constexpr auto kBoostMinorDivisor = 100;
constexpr auto kBoostMinorModulo = 1000;

} // namespace

/**
 * @brief 記録係の挨拶処理。
 * @details
 * 現在の実装では Boost のバージョン情報をデバッグログへ出力する。
 */
void historian::hello() const {
  BOOST_LOG_TRIVIAL(debug) << "Boost version (string): " << BOOST_LIB_VERSION;
  BOOST_LOG_TRIVIAL(debug) << "Boost version (numeric): "
                           << BOOST_VERSION / kBoostMajorDivisor << "."
                           << BOOST_VERSION / kBoostMinorDivisor %
                                  kBoostMinorModulo
                           << "." << BOOST_VERSION % kBoostMinorDivisor;
}

} // namespace core