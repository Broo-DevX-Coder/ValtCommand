// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "DevTools/Debugger.hpp"

// == Libs ==
#include <sstream>



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
// Debugger functions
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
    events_list.push_back(event);
    emit_slot(event);
}

// Set the slot of
void 
DevTools::Debugger::add_emit_slot(
    DevTools::Debugger::emitSlotType func
) {
    emit_slot = func;
}

// Get the str of next event aftre print_pos
std::string 
DevTools::Debugger::get_next_str() {
    print_pos++;
    return print_pos<events_list.size()? events_list[print_pos].get_str() : "EVENTS END";
}

// Get all strings of events
std::string 
DevTools::Debugger::get_all_strs() {
    std::stringstream ss;
    for (auto& event: events_list) {
        ss << event.get_str();
    }
    return ss.str();
}