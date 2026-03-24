#include "my_error.hpp"

namespace core::exceptions {

const char *my_error::what() const noexcept { return "my_error"; }

} // namespace core::exceptions