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
      cArgStruct argc;
      std::vector<string> argv;
      std::map<string,string> argm;
      virtual void argumentProcessor(string head, char splitter) { // Allow overriding if someone else wants to make a different processor
        for (const string& argument : argv) {
          if (argument.starts_with(head) && argument.contains(splitter)) {
            string name = argument.substr(head.length(), argument.find(splitter) - head.length());
            string content = argument.substr(argument.find(splitter) + 1);
            if (!name.empty() && !content.empty()) {
              argm.insert({name, content});
            }
          }
        }
      }
    public:
      arguments(int argc, char** argv): argc({argc, argv}) { // Store cArgs for later and legacy
        // Vectorise arguments
        for (int i = 0; i < this->argc.argc; i++) {
          this->argv.push_back(this->argc.argv[i]);
        }
        argumentProcessor("--", '=');
      };
      cArgStruct cArgs() { return argc; }
      map<string,string> map() { return argm; }
      vector<string> vector() { return argv; }
  };
}