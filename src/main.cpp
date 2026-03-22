#include "main.hpp"

int main(int argc, char *argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
  core::settings::options::instance().store(argc, argv);

  std::cout << core::hello();
  return 0;
}
