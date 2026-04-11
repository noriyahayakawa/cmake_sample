#include "core/exceptions/my_error.hpp"
#include "core/settings/options.hpp"
#include "core/utilities/text_writer_factory.hpp"

/**
 * @file flat_map.cpp
 * @brief `boost::container::flat_map` を利用するための実装ファイル。
 */

namespace core::utilities {

text_writer_factory::writer_result
text_writer_factory::writer(const std::string &name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = writers_.find(name);
  if (it != writers_.end()) {
    return {it->second, false};
  } else {
    return create_writer(name);
  }
}

text_writer_factory::writer_result
text_writer_factory::create_writer(const std::string &name) {
  fs::path file_path =
      core::settings::options::instance().input_file().commons.output_dir /
      name;
  bool created = !fs::exists(file_path);
  auto [it, inserted] = writers_.emplace(
      name, fs::ofstream(file_path.string(),
                         std::ios::app | std::ios::out | std::ios::binary));
  if (!it->second.is_open()) {
    writers_.erase(it);
    BOOST_THROW_EXCEPTION(
        core::exceptions::my_error{}
        << core::exceptions::errinfo_path(file_path.string())
        << core::exceptions::errinfo_message("Failed to open file: " + name));
  }
#ifdef _WIN32
  if (created) {
    // BOM付きUTF-8 (EF BB BF) を先頭に書き込む
    static constexpr unsigned char kUtf8Bom[] = {0xEF, 0xBB, 0xBF};
    it->second.write(reinterpret_cast<const char *>(kUtf8Bom),
                     sizeof(kUtf8Bom));
  }
#endif
  return {it->second, created};
}

} // namespace core::utilities