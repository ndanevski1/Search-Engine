#include <xapian.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
// #include "common.h"

using namespace std;

const string STOPWORD_FILE = "stopwords.txt";

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

void word_tokenize(string s, vector<string>& tokens){
    string punctuation = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ ";
    char is_punctuation[256];
    for(int i = 0; i < 256; i++){
        is_punctuation[i] = 0;
    }
    for(char c : punctuation){
        is_punctuation[c] = true;
    }

    string curr_word = "";
    cout << curr_word << '\n';
    for(char c : s){
        if(is_punctuation[c]){
            if(curr_word != ""){
                tokens.push_back(curr_word);
            }
            curr_word = "";
        } else {
            curr_word.push_back(c);
        }
    }
    if(curr_word != ""){
        tokens.push_back(curr_word);
    }
}

int main(int argc, char **argv) {
    if(argc < 3) {
        cerr << "Usage: metadata_indexer <dataset_metadata> <index_name>" << endl;
        exit(0);
    }

    char *dataset_title = argv[1];
    vector<string> lines;
    ifstream dataset(dataset_title);

    string reader;
    while(getline(dataset, reader)){
        lines.push_back(reader);
    }

    set<string> stopword_set = read_stopwords(STOPWORD_FILE);

    assert(lines.size() % 2 == 0);

    char *idxname = argv[2];
    Xapian::WritableDatabase db(idxname, Xapian::DB_CREATE_OR_OPEN);
    Xapian::Document doc;

    for(auto ite = lines.begin(); ite != lines.end(); ite++){
        string name = *ite;
        ite++;
        string description = *ite;

        doc.clear_terms();
        doc.clear_values();

        // add terms to document
        vector<string> tokens;
        word_tokenize(description, tokens);
        for(string token : tokens){
            if(!is_stopword(token, stopword_set)){
                doc.add_term(token);
            }
        }

        // add name to document
        doc.add_value(0, name);

        db.add_document(doc);
    }
}
