#ifndef COMMON
#define COMMON

#include <string>
#include <set>

using namespace std;

const string STOPWORD_FILE = "stopwords.txt";

set<string> read_stopwords(string file_name);
bool is_stopword(string s, set<string>& stopwords);
string word_highlighter(string& s, set<string>& highlight_words);

#endif
