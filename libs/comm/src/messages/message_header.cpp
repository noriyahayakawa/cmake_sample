#include "comm/messages/message_header.hpp"
#include <boost/endian/conversion.hpp>
#include <boost/throw_exception.hpp>
#include <algorithm>
#include <cstring>
#include <stdexcept>

/**
 * @file message_header.cpp
 * @brief TCP メッセージヘッダーのエンコード・デコード処理を実装する。
 */

namespace comm::messages {

/**
 * @brief メッセージ型文字列を設定する。
 * @param t 設定するメッセージ型文字列。
 * @throws std::logic_error 文字列幅が `type_size_` 以上の場合。
 */
    void message_header::message_type(const std::string &type) {
        if (type.size() >= type_size_) {
            boost::throw_exception(std::logic_error(
                "message_type: 文字列幅が最大サイズを超えています"));
        }
        message_type_ = type;
    }

/**
 * @brief ヘッダーデータを `data_` にビッグエンディアンでエンコードする。
 * @throws std::logic_error `message_type_` が空の場合。
 * @details
 * `message_length_`、`message_version_`、`message_type_` を各オフセット先に
 * ネットワークバイトオーダー（ビッグエンディアン）で書き込む。
 * `message_type_` は不足分を NUL パディングする。
 */
    void message_header::encode_header() {
        if (message_type_.empty()) {
            boost::throw_exception(std::logic_error("encode_header: message_type_ が空です"));
        }

        uint32_t len_be = boost::endian::native_to_big(message_length_);
        std::memcpy(data_.data() + length_offset_, &len_be, sizeof(len_be));

        uint16_t ver_be = boost::endian::native_to_big(message_version_);
        std::memcpy(data_.data() + version_offset_, &ver_be, sizeof(ver_be));

        auto* dst = reinterpret_cast<char*>(data_.data() + type_offset_);
        std::fill(dst, dst + type_size_, '\0');
        std::memcpy(dst, message_type_.data(), message_type_.size());
    }

/**
 * @brief `data_` のバイト列をヘッダーフィールドにデコードする。
 * @throws std::runtime_error `data_` の type 領域に NUL 終端が見つからない場合。
 * @details
 * `data_` 内の `length_offset_`、`version_offset_`、`type_offset_` 以降の
 * バイト列をルートエンディアンに変換して各フィールドに割り当てる。
 * type 領域に NUL が見つからない場合はデータ不正として例外を送出する。
 */
    void message_header::decode_header() {
        uint32_t len_be{};
        std::memcpy(&len_be, data_.data() + length_offset_, sizeof(len_be));
        message_length_ = boost::endian::big_to_native(len_be);

        uint16_t ver_be{};
        std::memcpy(&ver_be, data_.data() + version_offset_, sizeof(ver_be));
        message_version_ = boost::endian::big_to_native(ver_be);

        const auto* src = reinterpret_cast<const char*>(data_.data() + type_offset_);
        const auto* end = std::find(src, src + type_size_, '\0');
        if (end == src + type_size_) {
            boost::throw_exception(std::runtime_error(
                "decode_header: data_ に NUL 終端が見つかりません（データが不正です）"));
        }
        message_type_.assign(src, end);
    }

} // namespace comm::messages