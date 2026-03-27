#include "core.hpp"
#include <boost/version.hpp>

namespace core {

/**
 * @brief ライブラリ識別文字列を生成する。
 * @details `BOOST_VERSION`
 * を数値文字列として連結し、実行時に参照可能な形式で返す。
 * @return `hello (boost=<version>)` 形式の文字列。
 */
std::string hello() {
  return "hello (boost=" + std::to_string(BOOST_VERSION) + ")";
}

} // namespace core
