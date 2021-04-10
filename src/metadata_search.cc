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
#include <chrono>

#include "common.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {
    if(argc < 4) {
        cerr << "Usage: metadata_search <index_name> <top-k> <keyword1> <keyword2> ..." << endl;
        exit(0);
    }

    auto start_time = high_resolution_clock::now();
    
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

    Xapian::Query queryAND = Xapian::Query(
        Xapian::Query::OP_AND,
        queryAND_keywords.begin(),
        queryAND_keywords.end()
    );
    Xapian::Query queryOR = Xapian::Query(
        Xapian::Query::OP_OR,
        queryOR_keywords.begin(),
        queryOR_keywords.end()
    );

    Xapian::Query query;
    if(queryAND_keywords.empty()){
        query = queryOR;
    } else {
        query = Xapian::Query(
            Xapian::Query::OP_AND_MAYBE,
            queryAND,
            queryOR
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
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    std::cerr << "METADATA SEARCH TIME: " << duration.count() << " microseconds." << std::endl;

    return 0;
}
