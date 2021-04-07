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
// #include "common.h"

using namespace std;


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

    Xapian::Query query;
    
    if(queryOR_keywords.size() != 0) {
        query = Xapian::Query(
            Xapian::Query::OP_OR,
            queryOR_keywords.begin(),
            queryOR_keywords.end()
        );
    }

    for(auto keyword_and: queryAND_keywords) {
        if(queryOR_keywords.size() == 0) {
            query = Xapian::Query(
                Xapian::Query::OP_AND,
                queryAND_keywords.begin(),
                queryAND_keywords.end()
            );
        }
        else {
            query &= Xapian::Query(
                Xapian::Query::OP_AND,
                queryAND_keywords.begin(),
                queryAND_keywords.end()
            );
        }
    }
    



    Xapian::Enquire enquire(db);
    enquire.set_query(query);
    
    Xapian::MSet matches = enquire.get_mset(0, top_k); 
    printf("mset size is %d\n", matches.size());

    for(Xapian::MSetIterator match = matches.begin(); match != matches.end(); match ++) {
        Xapian::Document doc = match.get_document();    
        string value0 = doc.get_value(0);
        cout << value0 << endl;
    }

    return 0;
}
