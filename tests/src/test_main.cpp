#include "options.hpp"
#include "show_help.hpp"

#include <gtest/gtest.h>

TEST(options_test, store_with_help_option_throws_show_help) {
  char program[] = "tests";
  char help[] = "-h";
  char *argv[] = {program, help};

  try {
    core::settings::options::instance().store(2, argv);
    FAIL() << "show_help was not thrown";
  } catch (const core::exceptions::show_help &error) {
    const std::string *message =
        boost::get_error_info<core::exceptions::errinfo_message>(error);
    ASSERT_NE(message, nullptr);
    EXPECT_NE(message->find("--help"), std::string::npos);
    EXPECT_NE(message->find("--input"), std::string::npos);
  }
}