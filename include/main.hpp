#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "core/exceptions/my_error.hpp"
#include "core/exceptions/show_help.hpp"
#include "core/settings/options.hpp"
#include "core/student_council.hpp"
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/functional.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>

/**
 * @file main.hpp
 * @brief アプリケーション本体で使用する主要ヘッダの集約インクルード。
 */