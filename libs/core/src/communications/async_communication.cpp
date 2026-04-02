#include "communications/async_communication.hpp"
#include <boost/asio/awaitable.hpp>

namespace core::communications {

namespace asio = boost::asio;
using asio::ip::tcp;

async_communication::async_communication(asio::io_context &io_context)
    : io_context_(io_context), options_(settings::options::instance()) {}

asio::awaitable<tcp::resolver::results_type>
async_communication::resolve_once(std::string host, std::string service) {
  auto executor = co_await asio::this_coro::executor;
  tcp::resolver resolver(executor);
  tcp::resolver::results_type endpoints =
      co_await resolver.async_resolve(host, service, asio::use_awaitable);
  co_return endpoints;
}

} // namespace core::communications