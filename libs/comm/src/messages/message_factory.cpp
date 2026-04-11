#include "comm/messages/message_factory.hpp"

#include <stdexcept>

/**
 * @file message_factory.cpp
 * @brief TCP メッセージファクトリクラスの実装を提供する。
 */

namespace comm::messages {

/**
 * @brief 型文字列に対応するメッセージオブジェクトを生成する。
 * @param type メッセージ型を表す文字列キー。
 * @return 生成した `message` 派生オブジェクトの shared_ptr。
 * @throws std::runtime_error 未登録の型を指定した場合。
 */
std::shared_ptr<message> message_factory::create_message(const std::string &type) {
    auto it = creators_.find(type);
    if (it != creators_.end()) {
        return it->second();
    }
    boost::throw_exception(std::runtime_error("Unknown message type: " + type));
}

/**
 * @brief メッセージ型とそのクリエーター関数をファクトリーに登録する。
 * @param type    メッセージ型を表す文字列キー。
 * @param creator `message` 派生オブジェクトを生成するファクトリー関数。
 */
void message_factory::register_message(const std::string &type,
    boost::function<std::shared_ptr<message>()> creator) {
    creators_[type] = creator;
}

} // namespace comm::messages