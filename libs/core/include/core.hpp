#pragma once
#include <string>

namespace core {
/**
 * @brief ライブラリ識別用の文字列を返す。
 * @details
 * 戻り値にはライブラリ名と Boost のバージョン番号を含む。
 * バージョン番号は `BOOST_VERSION` マクロの値をそのまま文字列化して利用する。
 * @return `hello (boost=<version>)` 形式の文字列。
 */
std::string hello();
} // namespace core