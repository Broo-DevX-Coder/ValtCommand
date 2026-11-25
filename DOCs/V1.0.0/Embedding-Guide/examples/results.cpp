#include <valtcommand/valtcommand.hpp>
#include <cassert>
#include <sstream>
int main() {
  vltc::Runtime::RunTime rt(true);
  std::ostringstream trace;
  rt.set_debugger_ostream(trace);
  auto r = rt.run("SET answer<int> = 42");
  assert(r && rt.get_variable("answer").value == "42");
  auto v = rt.set_variable("host_value", "8", "int");
  assert(v && rt.get_variable("host_value").value == "8");
}
