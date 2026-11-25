#include <valtcommand/valtcommand.hpp>
#include <cassert>
#include <fstream>
int main() {
  vltc::Runtime::RunTime rt;
  auto a = rt.run("SET answer<int> = 2 + 3");
  assert(a && rt.get_variable("answer").value == "5");
  std::ofstream("running.vlt") << "SET file_answer<int> = 7";
  auto b = rt.run_file("running.vlt");
  std::remove("running.vlt");
  assert(b && rt.get_variable("file_answer").value == "7");
  auto bad = rt.run("RETURN missing_name");
  assert(!bad);
}
