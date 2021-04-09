#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <set>

using namespace std;

set<string> read_stopwords(string file_name);
bool is_stopword(string s, set<string>& stopwords);

#endif
