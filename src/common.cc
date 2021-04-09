#include "common.h"

#include <fstream>

set<string> read_stopwords(string file_name){
    ifstream file(file_name);

    set<string> stopwords;
    string reader;
    while(file >> reader){
        stopwords.insert(reader);
    }
    return stopwords;
}

bool is_stopword(string s, set<string>& stopwords){
    return stopwords.count(s);
}
