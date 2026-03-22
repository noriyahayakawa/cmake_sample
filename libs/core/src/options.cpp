#include "options.hpp"

namespace core::settings {

options::options() : desc_("Allowed options") {
  desc_.add_options()("help,h", "show help")(
      "input,i", po::value<std::string>(), "input file");
}

void options::store(int argc, char *argv[]) {
  po::store(po::parse_command_line(argc, argv, desc_), vm_);

  if (vm_.count("help")) {
    return;
  }

  po::notify(vm_);
}

} // namespace core::settings