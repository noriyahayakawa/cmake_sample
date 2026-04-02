#pragma once

#include "settings/options.hpp"
#include <boost/asio.hpp>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>

namespace core::communications {

namespace asio = boost::asio;
using asio::ip::tcp;

class async_communication {
protected:
  asio::io_context &io_context_; ///< 非同期処理の実行コンテキスト。
  settings::options &options_;

  asio::awaitable<tcp::resolver::results_type>
  resolve_once(std::string host, std::string service);

public:
  explicit async_communication(asio::io_context &io_context);

  virtual ~async_communication() = default;
};

} // namespace core::communications