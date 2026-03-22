#include "main.hpp"

int wmain(int argc, wchar_t *argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
  std::cout << core::hello();
  return 0;
}
