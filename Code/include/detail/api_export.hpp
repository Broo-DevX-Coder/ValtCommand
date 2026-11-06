#pragma once

/**
 * @file api_export.hpp
 * @brief Defines the shared-library symbol visibility macro.
 */

#if defined(BUILDING_COMPILER_DLL)
#  if defined(_WIN32)
#    define API __declspec(dllexport)
#  else
#    define API __attribute__((visibility("default")))
#  endif
#else
#  if defined(_WIN32)
#    define API __declspec(dllimport)
#  else
#    define API
#  endif
#endif
