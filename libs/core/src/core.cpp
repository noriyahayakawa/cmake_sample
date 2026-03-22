#include "core.hpp"
#include <boost/version.hpp>

namespace core {

std::string hello() {
  return "hello (boost=" + std::to_string(BOOST_VERSION) + ")";
}

} // namespace core
