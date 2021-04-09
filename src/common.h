#ifndef COMMON
#define COMMON

#include <string>
#include <set>

using namespace std;

set<string> read_stopwords(string file_name);
bool is_stopword(string s, set<string>& stopwords);

#endif
