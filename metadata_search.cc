#include <xapian.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cassert>
#include <map>
#include <cstdlib>

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

string word_highlighter(string& s, set<string>& highlight_words){
    string punctuation = "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ ";
    char is_punctuation[256];
    for(int i = 0; i < 256; i++){
        is_punctuation[i] = 0;
    }
    for(char c : punctuation){
        is_punctuation[c] = true;
    }

    string total = "";
    string curr_word = "";
    for(char c : s){
        if(is_punctuation[c]){
            if(curr_word != ""){
                if(highlight_words.count(curr_word)){
                    total += "033[1;31m";
                    total += curr_word;
                    total += "033[1;0m";
                } else {
                    total += curr_word;
                }
            }
            total += c;
            curr_word = "";
        } else {
            curr_word.push_back(c);
        }
    }
    if(curr_word != ""){
        if(highlight_words.count(curr_word)){
            total += "033[1;31m";
            total += curr_word;
            total += "033[1;0m";
        } else {
            total += curr_word;
        }
    }

    return total;
}

int main(int argc, char **argv) {
    if(argc < 4) {
        cerr << "Usage: metadata_search <index_name> <top-k> <keyword1> <keyword2> ..." << endl;
        exit(0);
    }
    Xapian::Database db(argv[1]);
    int top_k = atoi(argv[2]);
    vector<string> keywords;
    for(int i = 3; i < argc; i++)
        keywords.push_back(argv[i]);

    vector<string> queryAND_keywords, queryOR_keywords;

    for(auto keyword: keywords) {
        if(keyword[0] == '+')
            queryAND_keywords.push_back(keyword.substr(1, keyword.size()-1));
        else
            queryOR_keywords.push_back(keyword);
    }

    bool initialized_query = false;
    Xapian::Query query;

    if(queryOR_keywords.size() != 0) {
        initialized_query = true;
        query = Xapian::Query(
            Xapian::Query::OP_OR,
            queryOR_keywords.begin(),
            queryOR_keywords.end()
        );
    }

    assert(queryOR_keywords.size() > 0 or queryAND_keywords.size() > 0);
    if(!initialized_query and queryAND_keywords.size() > 0){
        initialized_query = true;
        query = Xapian::Query(
            Xapian::Query::OP_AND,
            queryAND_keywords.begin(),
            queryAND_keywords.end()
        );
    } else if (queryAND_keywords.size() > 0){
        query &= Xapian::Query(
            Xapian::Query::OP_AND,
            queryAND_keywords.begin(),
            queryAND_keywords.end()
        );
    }

    Xapian::Enquire enquire(db);
    enquire.set_query(query);

    Xapian::MSet matches = enquire.get_mset(0, top_k);
    printf("mset size is %d\n", matches.size());

    set<string> highlight_words;
    for(auto s : queryAND_keywords){
        highlight_words.insert(s);
    }
    for(auto s : queryOR_keywords){
        highlight_words.insert(s);
    }
    set<string> stopword_set = read_stopwords(STOPWORD_FILE);
    for(auto s : stopword_set){
        highlight_words.erase(s);
    }

    for(Xapian::MSetIterator match = matches.begin(); match != matches.end(); match ++) {
        Xapian::Document doc = match.get_document();
        string value0 = doc.get_value(0);
        cout << value0 << endl;
        string description = doc.get_value(1);
        cout << word_highlighter(description, highlight_words) << endl;
    }

    return 0;
}
