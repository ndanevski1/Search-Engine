#include <xapian.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <chrono>

#include "common.hpp"

using namespace std;
using namespace std::chrono;

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
    auto start_time = high_resolution_clock::now();

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
        doc.add_value(1, description);

        db.add_document(doc);
    }

    db.commit();
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    std::cerr << "INDEXING TIME: " << duration.count() << " microseconds." << std::endl;
}
