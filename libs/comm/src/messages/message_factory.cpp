#include "comm/messages/message_factory.hpp"

#include <stdexcept>

namespace comm::messages {

std::shared_ptr<message> message_factory::create_message(const std::string &type) {
    auto it = creators_.find(type);
    if (it != creators_.end()) {
        return it->second();
    }
    boost::throw_exception(std::runtime_error("Unknown message type: " + type));
}

void message_factory::register_message(const std::string &type, boost::function<std::shared_ptr<message>()> creator) {
    creators_[type] = creator;
}

} // namespace comm::messages