#ifndef REPAIR_H
#define REPAIR_H

#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include "../include/queue.h"
#include "../include/threaded_sequence.h"
#include "../include/utils.h"

using namespace std;


class Repair
{
private:
    QUEUE q;
    unordered_map<pair<st, st>, PAIR*, PairHash> ht;
    TSEQ seq;
    st rule;
    vector<st> ruleHistory;

    void findAdjPairs(vector<st>& pairPos, st position);
    void decreaseAdjToPair(st pos);
    void replace(st position);
    void insert(PAIR* p, st current_position);
    void firstPass();
    void compactify();
    void compress(bool verbose);
    void printHashTable();
    public:
    
    Repair(const string& input);
    ~Repair();
    void run(bool verbose = false)
    {
        firstPass();
        compress(verbose);
    }
    void output();
};

 
#endif // REPAIR_H