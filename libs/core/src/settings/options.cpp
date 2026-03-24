#include "settings/options.hpp"
#include "exceptions/my_error.hpp"
#include "exceptions/show_help.hpp"
#include <array>
#include <boost/json.hpp>
#include <fstream>
#include <stdexcept>
#include <system_error>

namespace core::settings {

namespace json = boost::json;

options::options() : desc_("利用可能なオプション") {
  desc_.add_options()("help,h", "ヘルプを表示")(
      "input,i", po::value<fs::path>()->default_value("input.jsonc"),
      "入力ファイル");
}

void options::store(int argc, char *argv[]) {

  executable_path_ = fs::absolute(argv[0]);

  po::positional_options_description pos;
  pos.add("input", 1);
  po::store(
      po::command_line_parser(argc, argv).options(desc_).positional(pos).run(),
      vm_);
  if (vm_.count("help")) {
    BOOST_THROW_EXCEPTION(core::exceptions::show_help{}
                          << core::exceptions::errinfo_message(help_text()));
  }
  po::notify(vm_);

  if (vm_.count("input")) {
    input_file_path_ = vm_["input"].as<fs::path>();
    input_file_path_.is_relative()
        ? input_file_path_ =
              executable_path_.parent_path() / input_file_path_.filename()
        : input_file_path_;
    boost::system::error_code ec;
    if (!is_regular_file_nothrow(input_file_path_, ec)) {
      input_file_path_ = input_file_path_.parent_path().parent_path() / "etc" /
                         input_file_path_.filename();
      if (!is_regular_file_nothrow(input_file_path_, ec)) {
        input_file_path_ =
            executable_path_.parent_path() / input_file_path_.filename();
        BOOST_THROW_EXCEPTION(
            core::exceptions::my_error{}
            << core::exceptions::errinfo_message(
                   "入力ファイルが存在しないか、通常のファイルではありません。")
            << core::exceptions::errinfo_path(input_file_path_.string())
            << core::exceptions::errinfo_errno(ec.value()));
      }
    }
  }
}

bool options::read_input_file() {
  try {
    std::ifstream ifs(input_file_path_.string(), std::ios::binary);
    if (!ifs) {
      throw boost::system::system_error(
          boost::system::error_code(errno, boost::system::generic_category()),
          "ファイルを開けませんでした");
    }

    json::stream_parser parser;
    boost::system::error_code ec;

    bool first_chunk = true;
    std::array<char, 4096> buffer{};
    while (ifs) {
      ifs.read(buffer.data(), buffer.size());

      std::streamsize read = ifs.gcount();
      if (read <= 0) {
        break;
      }

      const char *data = buffer.data();
      std::size_t size = static_cast<std::size_t>(read);

      // ---- BOM 除去（先頭チャンクのみ）----
      if (first_chunk) {
        first_chunk = false;
        if (size >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
            static_cast<unsigned char>(data[1]) == 0xBB &&
            static_cast<unsigned char>(data[2]) == 0xBF) {
          data += 3;
          size -= 3;
        }
      }

      parser.write(data, size, ec);
      if (ec) {
        throw std::runtime_error("json parse error: " + ec.message());
      }
    }

    parser.finish(ec);
    if (ec) {
      throw std::runtime_error("json parse error: " + ec.message());
    }

    json::value jv = parser.release();
    const auto &obj = jv.as_object();
    if (obj.contains("commons")) {
      commons_ = boost::json::value_to<commons>(jv.at("commons"));
    }

    return true;
  } catch (const std::exception &e) {
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_message(e.what())
        << core::exceptions::errinfo_path(input_file_path_.string()));
    return false;
  }
}

std::string options::help_text() {
  std::ostringstream oss;
  oss << desc_;
  return oss.str();
}

bool options::is_regular_file_nothrow(const fs::path &p,
                                      boost::system::error_code &ec) {
  ec.clear();
  fs::file_status st = fs::status(p, ec);
  if (ec)
    return false;
  return fs::is_regular_file(st);
}

} // namespace core::settings