/**
 * @file api_export.hpp
 * @brief Defines the ValtCommand shared-library symbol visibility macro.
 */
#pragma once

#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define VALTC_API __declspec(dllexport)
    #else
        #define VALTC_API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define VALTC_API __declspec(dllimport)
    #else
        #define VALTC_API
    #endif
#endif
