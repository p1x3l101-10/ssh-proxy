#pragma once
#include <vector>
#include <string>
#include <map>

namespace arguments {
  using std::vector,std::string,std::map;
  class arguments {
    protected:
      struct cArgStruct {
        int argc;
        char** argv;
      };
    private:
      cArgStruct cArgs;
      std::vector<string> argv;
      std::map<string,string> argm;
    public:
      arguments(int argc, char** argv): cArgs({argc, argv}) { // Store cArgs for later and legacy
        // Vectorise arguments
        for (int i = 0; i < cArgs.argc; i++) {
          this->argv.push_back(cArgs.argv[i]);
        }
    
        // Turn vector into argument map
        for (const string& argument : this->argv) {
          // Must match pattern of `--argName=argValue`
          if (argument.starts_with("--") && argument.contains("=")) {
            string name = argument.substr(2, argument.find("=") - 2);
            string content = argument.substr(argument.find("=") + 1);
            // Ensure that there is no issues
            if (!name.empty() && !content.empty()) {
              argm.insert({name, content});
            }
          }
        }
      };
      map<string,string> map() { return argm; }
      vector<string> vector() { return argv; }
  };
}