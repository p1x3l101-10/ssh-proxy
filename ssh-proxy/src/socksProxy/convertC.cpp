#include "ssh-proxy.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using std::vector;
using std::string;

char* strToChar(const string& s) {
    char* pc = new char [s.size()+1];
    std::strcpy(pc, s.c_str());
    return pc;
}

char** sshProxy::socksProxy::convertC(vector<string> stringList) {
    vector<char*> vecCstr;
    std::transform(stringList.begin(),stringList.end(), std::back_inserter(vecCstr), strToChar);
    vecCstr.push_back(NULL);
    char** arrCstr;
    std::copy(vecCstr.begin(), vecCstr.end(), arrCstr);
    return arrCstr;
}