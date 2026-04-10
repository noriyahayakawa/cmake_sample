#include "main.hpp"

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[]) {
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
int main(int argc, char *argv[]) {
#endif
  std::cout << core::hello();
  return 0;
}
