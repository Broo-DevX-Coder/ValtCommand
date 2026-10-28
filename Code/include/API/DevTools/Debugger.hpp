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

    // The event struct
    struct event {
        EventsT type; // The level of event
        std::string message; // Message of event
        size_t line; // Line of event

        // print event
        std::string get_str() {
            return fmt::format("[{}] line:{} -> {}\n", event_type_to_string(type), line, message);
        };
    };

    // The breakpoit struct type
    struct breakPointType {
        private:
            size_t line; // The line of breakpoint
            bool avilable = true; // Is the breakpoint avilable to stop

            bool temp = false; // is the breakpoint temp
            bool used = false; // did the breakpoint used

        public:
            breakPointType(size_t l_) {line=l_;}
            breakPointType(size_t l_,bool temp) {line=l_;temp=temp;} // Constructor of temp types

            void set_av__(bool av_){avilable = av_;} // Set avilability of breakpointr

            void use(){used=temp;} // use the breakpoint if it is temp
            bool is_used() const {return used;} // Is the breakpoin used

            explicit operator bool() const {return avilable && !used;}
            explicit operator size_t() const {return line;}
    };


    // The debugger class
    class Debugger {
            using emitSlotType = std::function<void(event)>; // The type of emit slot's function
            using stopSlotType = std::function<void(Debugger&)>; // The type of slot when code stops
            
        private:

            // Geniral ============

            bool steal_running = true; // Is the code steal running
            bool debug_mode = false; // Is the debugger allowed
            std::ostream* output_stream = &std::cout; // The general output stream of all lang code, it can be std::cout or any thing else

            // RunTime tracker ==========

            std::vector<event> events_list; // The list of all events that saved in str
            size_t print_pos = 0; // The print position that shows the print function what it is
    
            static void print_event(event event); // Print a event's string
            emitSlotType emit_slot = print_event;

            // Breakpoints =============

            std::vector<breakPointType> breakpoints; // The breakpoints list
            size_t curent_line; // the number of curent line emited

            static void stop_slot(Debugger& debuger); // When breakpoint activated, it will shows `Enter to continue ...`
            stopSlotType stopCodeSlot = stop_slot; // Teh slot when the code stops


        public:

            // General =============
            Debugger() = default; // Normal constructure
            Debugger(std::ostream& ostream){output_stream = &ostream;} // Contructure sepports set ostream

            void set_ostream(std::ostream& ostream){output_stream=&ostream;} // Set the output stream of debugger
            const std::ostream* get_ostream(){return output_stream;} // Get output stream

            // Runtime tracker =============

            void emit(event event); // emit event
            void set_emit_event_slot(emitSlotType func); // Set the slot of 
            std::string get_next_event_str(); // Get the str of next event aftre pos
            std::string get_all_events_strs(); // Get all strings of events

            // Breakpoints ============

            // Managinng breakpoints
            void add_break_points(const std::vector<size_t>& points_buffer); // Add breakpoints like a buffer
            void check_break_point(size_t line); // Verifi if need to stop in curent line and stop if needed
            void set_stop_slot(stopSlotType slot); // Set the function that activated when the breakpoint activated or any other event thaat stops the code execution

            // Managing debugging
            void continue_exec();
            void next_line(); // Move to the next line
            void exit(); // Exit the program
    };
}