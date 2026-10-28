// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "API/DevTools/Debugger.hpp"

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
        CONTINUE, // contenue executing 
        NEXT_LINE, // Move to the next line
        EXIT, // Exit all the code
        UNKNOWN // Unknown command
    };

    // All command in unordered map
    std::unordered_map<std::string,COMMANDT> commands_map{
        {"continue",CONTINUE},
        {"nextl",NEXT_LINE},
        {"exit",EXIT},

        {"c",CONTINUE},
        {"n",NEXT_LINE},
        {"e",EXIT}
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
    if (event.type != DevTools::EventsT::Analysis && event.line != curent_line) {
        curent_line = event.line;
        check_break_point(event.line);
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
    const std::vector<size_t>& points_buffer
) {
    for (auto& p: points_buffer) {
        breakpoints.push_back(breakPointType(p));
    }
}


void
DevTools::Debugger::check_break_point(
    size_t line
) {
    for (size_t i = 0; i < breakpoints.size(); ++i) {

        const bool matched =
            line == static_cast<size_t>(breakpoints[i]) &&
            static_cast<bool>(breakpoints[i]);

        if (matched) {
            breakpoints[i].set_av__(false);
            breakpoints[i].use();

            stopCodeSlot(*this);
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
    DevTools::Debugger& debuger
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
    breakpoints.push_back(breakPointType(curent_line+1,true));
}

// Continue executing
void 
DevTools::Debugger::continue_exec() {}

// Exit the program
void 
DevTools::Debugger::exit() {
    steal_running = false;
}

// Set the function that activated when the breakpoint activated or any other event thaat stops the code execution
void 
DevTools::Debugger::set_stop_slot(
    stopSlotType slot
) {
    stopCodeSlot = slot;
}
 