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
      struct argumentIdent {
        string head;
        string delimiter;
      };
    private:
      const cArgStruct argc;
      const char** envc;
      std::vector<string> argv;
      std::map<string,string> argm;
      void argumentVectorizer() {
        for (int i = 0; i < this->argc.argc; i++) {
          this->argv.push_back(this->argc.argv[i]);
        }
      }
      virtual void argumentMapper(argumentIdent ident) { // Allow overriding if someone else wants to make a different processor
        for (const string& argument : argv) {
          if (argument.starts_with(ident.head) && argument.contains(ident.delimiter)) {
            string name = argument.substr(ident.head.length(), argument.find(ident.delimiter) - ident.head.length());
            string content = argument.substr(argument.find(ident.delimiter) + ident.delimiter.length());
            if (!name.empty() && !content.empty()) {
              argm.insert({name, content});
            }
          }
        }
      }
    public:
      arguments(int argc, char** argv, char** envp = {}, argumentIdent delimiters = {"--", "="}): argc({argc, argv}) {
        argumentVectorizer();
        argumentMapper(delimiters);
      };
      cArgStruct cArgs() { return argc; }
      map<string,string> map() { return argm; }
      vector<string> vector() { return argv; }
  };
}