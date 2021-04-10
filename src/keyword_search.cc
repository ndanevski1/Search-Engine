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

using namespace std;

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


vector<set<string>> create_sets_to_compare(Xapian::Database db, int set_cardinality, int number_of_sets) {
    vector<set<string>> v;
    srand(time(0));
    for(int i = 0; i < number_of_sets; i++)
        v.push_back(create_random_set(db, set_cardinality));
    return v;
}
int main(int argc, char **argv) {
    if(argc < 4) {
        cerr << "Usage: keyword_search <index_name> <top-k> <set_cardinality>" << endl;
        exit(0);
    }
    Xapian::Database db(argv[1]);
    int top_k = atoi(argv[2]);
    int set_cardinality = atoi(argv[3]);

    set<string> query_set = create_random_set(db, set_cardinality);
    int number_of_sets = 30;
    vector<set<string>> sets_to_compare = create_sets_to_compare(db, set_cardinality, number_of_sets);

    cout << "Printing query set: ";
    for(auto s: query_set)
        cout << s << " ";

    cout << endl;
    for(auto v: sets_to_compare) {
        for(auto s: v)
            cout << s << " ";
        cout << endl;
    }

    return 0;
}
