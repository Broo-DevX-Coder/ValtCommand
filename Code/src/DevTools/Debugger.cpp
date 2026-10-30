// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "API/DevTools/Debugger.hpp"
#include "API/DevTools/Inspectures.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Helper: Turn event type to string
// ==================================================================

std::string 
DevTools::event_type_to_string(
    DevTools::EventsT category
) {
    switch (category) {
        case EventsT::Info:     return "Info";
        case EventsT::Analysis: return "Analysis";
        case EventsT::Runtime:  return "Runtime";
        case EventsT::Debug:    return "Debug";
        case EventsT::Warning:  return "Warning";
        case EventsT::Error:    return "Error";
    }

    return "Unknown";
}

// ==================================================================
// Helpers
// ==================================================================
namespace DevTools {
namespace {

    // All commands types should user enter them after stoping code
    enum COMMANDT {

        // Basic Commands =====

        CONTINUE, ///< contenue executing 
        NEXT_LINE, ///< Move to the next line
        EXIT, ///< Exit all the code

        // Inspect methods =====
        INSPECT_VARS, // Print the vars table
        INSPECT_FUNCTIONS, // Print functions table
        INSPECT_SCOPE, // Print scope info

        UNKNOWN ///< Unknown command
    };

    // All command in unordered map
    std::unordered_map<std::string,COMMANDT> commands_map{
        {"continue",CONTINUE},
        {"nextl",NEXT_LINE},
        {"exit",EXIT},
        {"show vars",INSPECT_VARS},
        {"show functions",INSPECT_FUNCTIONS},
        {"show scope info",INSPECT_SCOPE},

        {"c",CONTINUE},
        {"n",NEXT_LINE},
        {"e",EXIT},
        {"sv",INSPECT_VARS},
        {"sf",INSPECT_FUNCTIONS},
        {"sc",INSPECT_SCOPE},
    };

    COMMANDT command_2enum(const std::string& command) {
        if (!commands_map.contains(command)) {
            return UNKNOWN;
        }
        return commands_map[command];
    }
}
}


// ==================================================================
// Runtime tracker
// ==================================================================

// Print event 
void 
DevTools::Debugger::print_event(
    event event
){
    std::cout << event.get_str();
}

// Emit event
void
DevTools::Debugger::emit(
    event event
) {
    curent_file = event.file;
    if (event.type != DevTools::EventsT::Analysis && event.line != curent_line) {
        curent_line = event.line;
        check_break_point(event.line, event.curent_scope, event.file);
    }
    events_list.push_back(event);
    emit_slot(event);
}

// Set the slot of
void 
DevTools::Debugger::set_emit_event_slot(
    DevTools::Debugger::emitSlotType func
) {
    emit_slot = func;
}

// Get the str of next event aftre print_pos
std::string 
DevTools::Debugger::get_next_event_str() {
    print_pos++;
    return print_pos<events_list.size()? events_list[print_pos].get_str() : "EVENTS END";
}

// Get all strings of events
std::string 
DevTools::Debugger::get_all_events_strs() {
    std::stringstream ss;
    for (auto& event: events_list) {
        ss << event.get_str();
    }
    return ss.str();
}


// ==================================================================
// Breakpoint managing
// ==================================================================

// Add breakpoints like a buffer
void 
DevTools::Debugger::add_break_points(
    const std::vector<size_t>& points_buffer,
    const std::string& FilePath
) {
    for (auto& p: points_buffer) {
        breakpoints.push_back(breakPointType(p, FilePath));
    }
}


void
DevTools::Debugger::check_break_point(
    size_t line,
    Scopes::Scope* scope,
    const std::string& FilePath
) {
    for (size_t i = 0; i < breakpoints.size(); ++i) {

        const bool matched =
            line == static_cast<size_t>(breakpoints[i]) &&
            (breakpoints[i].get_file().empty() || breakpoints[i].get_file() == FilePath) &&
            static_cast<bool>(breakpoints[i]);

        if (matched) {
            breakpoints[i].set_av__(false);
            breakpoints[i].use();

            stopCodeSlot(*this, scope);
        } else {
            breakpoints[i].set_av__(true);
        }
    }

    breakpoints.erase(
        std::remove_if(
            breakpoints.begin(),
            breakpoints.end(),
            [](const DevTools::breakPointType& bp) {
                return bp.is_used();
            }
        ),
        breakpoints.end()
    );
}



void
DevTools::Debugger::stop_slot(
    DevTools::Debugger& debuger,
    Scopes::Scope* scope
) {
    bool waiting = true;

    while (waiting) {
        std::string command;
        std::cout << "> " << std::flush;

        if (!std::getline(std::cin, command)) {
            debuger.exit();

            break;
        }

        switch (command_2enum(command)) {
            case CONTINUE:
                debuger.continue_exec();
                waiting = false;
                break;

            case NEXT_LINE:
                debuger.next_line();
                waiting = false;
                break;

            case EXIT:
                debuger.exit();
                waiting = false;
                break;

            case INSPECT_SCOPE:
                std::cout << ScopeInspect(scope).printable;
                break;

            case INSPECT_VARS:
                std::cout << VariablesInspect(scope).printable;
                break;

            case INSPECT_FUNCTIONS:
                std::cout << FunctionsInspect(scope).printable;
                break;

            default:
                std::cout
                    << "Command `" << command
                    << "` not found!\n";
                break;
        }
    }
}


// Move to the next line
void 
DevTools::Debugger::next_line() {
    breakpoints.push_back(breakPointType(curent_line + 1, curent_file, true));
}

// Continue executing
void 
DevTools::Debugger::continue_exec() {}

// Exit the program
void 
DevTools::Debugger::exit() {
    std::cout << "Code exited successfuly\n";
    std::exit(0);
}

// Set the function that activated when the breakpoint activated or any other event thaat stops the code execution
void 
DevTools::Debugger::set_stop_slot(
    stopSlotType slot
) {
    stopCodeSlot = slot;
}