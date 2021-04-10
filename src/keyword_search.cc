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
#include <random>
#include <chrono>

#include "common.hpp"

using namespace std;
using namespace std::chrono;

int get_random_number(int high) {
    return rand() % high;
}
set<string> create_random_set(Xapian::Database db, int set_cardinality) {
    set<string> s;
    int doc_count = db.get_doccount();
    for(int i = 0; i < set_cardinality; i++) {
        int random_doc = get_random_number(doc_count);
        Xapian::Document doc = db.get_document(random_doc);
        int doc_terms_count = doc.termlist_count();
        int random_term_index = get_random_number(doc_terms_count);
        string random_term = "";
        auto ite = doc.termlist_begin();
        for(int j = 0; j < random_term_index; j++){
            ite++;
            if(ite == doc.termlist_end()){
                break;
            }
        }
        assert(ite != doc.termlist_end());
        random_term = *ite;
        s.insert(random_term);
    }
    return s;
}

int main(int argc, char **argv) {
    if(argc < 5) {
        cerr << "Usage: keyword_search <index_name> <top-k> <set_cardinality> <repeat_count>" << endl;
        exit(0);
    }
    auto start_time = high_resolution_clock::now();
    Xapian::Database db(argv[1]);
    int top_k = atoi(argv[2]);
    int set_cardinality = atoi(argv[3]);
    int repeat_count = atoi(argv[4]);

    srand(time(0));
    for(int repeats = 1; repeats <= repeat_count; repeats++){
        set<string> query_set = create_random_set(db, set_cardinality);
        cout << "Printing query set: ";
        for(auto s: query_set)
            cout << s << " ";
        cout << '\n';

        Xapian::Query query = Xapian::Query(
            Xapian::Query::OP_OR,
            query_set.begin(),
            query_set.end()
        );

        Xapian::Enquire enquire(db);
        enquire.set_query(query);
        Xapian::CoordWeight weighting_scheme = Xapian::CoordWeight();
        weighting_scheme.init(1.0);
        enquire.set_weighting_scheme(weighting_scheme);

        Xapian::MSet matches = enquire.get_mset(0, top_k);
        printf("mset size is %d\n", matches.size());

        set<string> highlight_words = query_set;
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

    }
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    std::cerr << "SIMILARITY SEARCH TIME: " << duration.count() << " microseconds." << std::endl;
    return 0;
}
