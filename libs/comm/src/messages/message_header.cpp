#include "comm/messages/message_header.hpp"
#include <boost/endian/conversion.hpp>
#include <boost/throw_exception.hpp>
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace comm::messages {

    void message_header::message_type(const std::string &t) {
        if (t.size() >= type_size_) {
            boost::throw_exception(std::logic_error("message_type: 文字列幅が最大サイズを超えています"));
        }
        message_type_ = t;
    }

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
            boost::throw_exception(std::runtime_error("decode_header: data_ に NUL 終端が見つかりません（データが不正です）"));
        }
        message_type_.assign(src, end);
    }
}