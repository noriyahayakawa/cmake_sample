#pragma once

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>
#include <string>

namespace core::exceptions {

using errinfo_path = boost::error_info<struct tag_path, std::string>;
using errinfo_errno = boost::error_info<struct tag_errno, int>;
using errinfo_message = boost::error_info<struct tag_msg, std::string>;

struct my_error : virtual boost::exception, virtual std::exception {
  const char *what() const noexcept override;
};

} // namespace core::exceptions