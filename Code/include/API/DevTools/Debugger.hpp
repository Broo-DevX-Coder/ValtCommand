#pragma once

// ==================================================================
// Marcos
// ==================================================================
#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define API __declspec(dllimport)
    #else
        #define API 
    #endif
#endif

// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "API/globals.hpp"

// == Libs ==
#include <functional>

// ==================================================================
// Namespace
// ==================================================================

namespace DevTools {

    // All levels of evens
    enum class EventsT {
        Info,
        Analysis,  // In semantic analyses step (verifi all codes berof running)
        Runtime, // Events when runing
        Debug, // Debuger events (breakpoint ...)
        Warning, // A warning
        Error // Errors that staped the program
    };

    std::string event_type_to_string(EventsT tegory); // Convert event type to string

    struct event {
        EventsT type; // The level of event
        std::string message; // Message of event
        size_t line; // Line of event

        // print event
        std::string get_str() {
            return fmt::format("[{}] line:{} -> {}\n", event_type_to_string(type), line, message);
        };
    };


    // The debugger class
    class Debugger {
            using emitSlotType = std::function<void(event)>;
        private:
            std::vector<event> events_list; // The list of all events that saved in str
            size_t print_pos = 0; // The print position that shows the print function what it is
    
            static void print_event(event event); // Print a event's string
            emitSlotType emit_slot = print_event;

        public:
            void emit(event event); // emit event
            void add_emit_slot(emitSlotType func); // Set the slot of 
            std::string get_next_str(); // Get the str of next event aftre pos
            std::string get_all_strs(); // Get all strings of events
    };
}