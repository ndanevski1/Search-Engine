#include "common.hpp"

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
                    total += "\033[1;31m";
                    total += curr_word;
                    total += "\033[1;0m";
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
            total += "\033[1;31m";
            total += curr_word;
            total += "\033[1;0m";
        } else {
            total += curr_word;
        }
    }

    return total;
}
