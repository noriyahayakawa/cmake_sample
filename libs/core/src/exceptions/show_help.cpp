#include "show_help.hpp"

namespace core::exceptions {

const char *show_help::what() const noexcept {
  if (const std::string *msg = boost::get_error_info<errinfo_message>(*this)) {
    return msg->c_str();
  }
  return "show_help";
}

} // namespace core::exceptions