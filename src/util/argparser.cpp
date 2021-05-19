
#include <string>
#include "util/argparser.hpp"

using json = nlohmann::json;
using string = std::string;

json argparser(int argc, char **argv) {
  json result;
  for (int i = 1; i < argc; ++i) {
    string arg = string(argv[i]);
    if (arg.substr(0, 2) == "--") {
      string key = arg.substr(2);
    } else if (arg.substr(0, 1) == "-") {
      string key = arg.substr(1);
    }
  }
  result["ERR"] = "this feature has not done yet.";
  return result;
}
