#include <valtcommand/valtcommand.hpp>
#include <cassert>
#include <sstream>
int main() {
  vltc::Runtime::RunTime rt(true);
  auto* debugger = rt.GetDebugger();
  assert(debugger);
  std::ostringstream events;
  rt.set_debugger_ostream(events);
  debugger->set_emit_event_slot([](vltc::DevTools::event e, std::ostream& out){out << e.get_str();});
  debugger->set_stop_slot([](vltc::DevTools::Debugger& d, vltc::Scopes::Scope*){d.continue_exec();});
  debugger->add_break_points({1});
  auto r=rt.run("SET answer<int> = 3");
  assert(r && rt.get_variable("answer").value=="3");
  assert(!debugger->get_all_events_strs().empty());
}
