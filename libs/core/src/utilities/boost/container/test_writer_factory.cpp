#include "core/exceptions/my_error.hpp"
#include "core/settings/options.hpp"
#include "core/utilities/text_writer_factory.hpp"

#include <array>

/**
 * @file flat_map.cpp
 * @brief `boost::container::flat_map` を利用するための実装ファイル。
 */

namespace core::utilities {

auto text_writer_factory::writer(const std::string &name)
    -> text_writer_factory::writer_result {
  std::scoped_lock lock(mutex_);
  auto writer_it = writers_.find(name);
  if (writer_it != writers_.end()) {
    return {.writer = writer_it->second, .created = false};
  }
  return create_writer(name);
}

auto text_writer_factory::create_writer(const std::string &name)
    -> text_writer_factory::writer_result {
  fs::path file_path =
      core::settings::options::instance().input_file().commons.output_dir /
      name;
  bool created = !fs::exists(file_path);
  auto [writer_it, inserted] = writers_.emplace(
      name, fs::ofstream(file_path.string(),
                         std::ios::app | std::ios::out | std::ios::binary));
  if (!writer_it->second || !writer_it->second.is_open()) {
    writers_.erase(writer_it);
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_path(file_path.string())
        << core::exceptions::errinfo_message("Failed to open file: " + name));
  }
#ifdef _WIN32
  if (created) {
    static constexpr std::array<unsigned char, 3> kUtf8Bom = {0xEFU, 0xBBU,
                                                              0xBFU};
    writer_it->second.write(reinterpret_cast<const char *>(kUtf8Bom.data()),
                            static_cast<std::streamsize>(kUtf8Bom.size()));
  }
#endif
  return {.writer = writer_it->second, .created = created};
}

} // namespace core::utilities