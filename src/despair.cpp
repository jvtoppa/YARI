#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <vector>
#include <fstream>
#include "../include/utils.h"
using namespace std;

struct paired
{
    size_t left;
    size_t right;
};


vector<size_t> read_sequence(bool verbose)
{
    ifstream file("output/sequence.rp", ios::binary);
    size_t val;
    vector<size_t> read;

    while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
    {
        read.push_back(val);

        if(!verbose)
        continue;

        if(val <= 255)
        cout <<  static_cast<char>(val);
        else
        cout << "[" << val << "]";
    }
    cout << "\n\n";

    return read;
}

vector<size_t> read_rulehistory()
{
    ifstream file("output/rulehistory.rp", ios::binary);
    size_t val;
    vector<size_t> read;
        
    while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
    {
        read.push_back(val);
    }

    return read;
}

void expand(size_t symbol, const vector<paired>& rules, vector<size_t>& out) {
    
    if (symbol <= 255)
    {
        out.push_back(symbol);
    } 
    else
    {
        expand(rules[symbol - 256].left, rules, out);
        expand(rules[symbol - 256].right, rules, out);
    }
}

int main()
{
    cout << "File read: sequence.rp";
    vector<size_t> sequence = read_sequence(false);
    cout << "File read: rulehistory.rp\n\n";
    vector<size_t> rulehistory = read_rulehistory();

  //  cout << rulehistory << "\n\n";
    
    vector<paired> rules;
    rules.reserve(rulehistory.size() / 2);

    for(size_t i = 0; i < rulehistory.size(); i += 2)
    {
        paired p;
        p.left = rulehistory[i];
        p.right = rulehistory[i+1];
        rules.push_back(p);
    }
    
    vector<size_t> decoded_sequence;
    decoded_sequence.reserve(sequence.size() * 2);

    for (size_t symbol : sequence)
    {
        expand(symbol, rules, decoded_sequence);
    }
    
    ofstream fout;
    
    fout.open("decoded.txt", ios::binary | ios::out | ios::trunc);

    for(const auto& character : decoded_sequence)
    {
        fout.write(reinterpret_cast<const char*>(&character), sizeof(char));
    }
    
    cout << "\n";
    return 0;
}
