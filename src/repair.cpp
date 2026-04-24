
#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include "Repair.h"

using namespace std;

Repair::Repair(const std::string& input) : q(input), seq(input), rule(255){}

Repair::~Repair()
{
    for (auto& kv : ht)
    {
        delete kv.second;
    }
    ht.clear();
}


void Repair::printHashTable()
{
    for (const auto &entry : ht)
    {
        cout << "(" << entry.first.first << ", " << entry.first.second << ") "
             << "-> freq: " << entry.second->freq
             << ", f_pos: " << (entry.second->f_pos == N ? "NULL" : to_string(entry.second->f_pos))
             << ", b_pos: " << (entry.second->b_pos == N ? "NULL" : to_string(entry.second->b_pos))
             << endl;
    }
}

void Repair::output()
{
    cout << "\nCompressed text: ";
    for (int i = 0; i < seq.size(); i++)
    {
        if(seq[i].code != N)
        {
            if(seq[i].code < 128)
            {
                cout << static_cast<char>(seq[i].code);
            }
            else
            {
                cout << "["  << static_cast<int>(seq[i].code) << "]";
            }
        }
    }
    cout << "\nRule History: ";
    cout << ruleHistory << "\n";
    cout << "\n";

}   

void Repair::insert(PAIR* p, st current_position)
{
    /*
    Inserts a pair in the data structures. If this is the first time
    seeing a pair, add its ref to the hash map. If this is the second
    time you're seeing the pair, add its ref to the priority queue.
    Else, increase the value of its frequency by 1 and adjust its 
    position in the queue.
    */

    auto key = make_pair(p->left, p->right);
    auto it = ht.find(key);

    if (it == ht.end())
    {
        p->f_pos = current_position;
        p->b_pos = current_position;
        ht[key] = p;
    }
    else
    {
        PAIR* existing = it->second;
        st prev_pos = existing->b_pos;
    
        seq[prev_pos].next = current_position;
        seq[current_position].prev = prev_pos;
        
        q.removePair(existing);

        existing->freq++;
        existing->b_pos = current_position;

        q.addPair(existing);

        delete p;
    }
}


void Repair::firstPass() 
{   
    /*
    Adds all pairs to the hash table, Priority queue
    and threads the SEQ vector.
    */

    st string_size = seq.size();
    
    unordered_map<pair<st, st>, st, PairHash> last_occurrence;
    
    for (st i = 0; i < string_size - 1; i++) 
    {
        pair<st, st> current_pair = {seq[i].code, seq[i+1].code};
        
        PAIR* new_pair = new PAIR;
        new_pair->left = current_pair.first;
        new_pair->right = current_pair.second;
        insert(new_pair, i);
        
        if (last_occurrence.find(current_pair) != last_occurrence.end())
        {
            st prev_pos = last_occurrence[current_pair];
            seq[i].prev = prev_pos;            
            seq[prev_pos].next = i;            
        }
        else
        {
            seq[i].prev = N;
        }
        seq[i].next = N;
        last_occurrence[current_pair] = i;
    }

    //if its the last element, it does not have a pair
    if (!seq.empty())
    {
        seq[string_size - 1].prev = N;
        seq[string_size - 1].next = N;
    }
}




void Repair::findAdjPairs(vector<st>& pairPos, st position)
{
    /*
    Locates adjacent pairs relative to position.
    Helper function for decreaseAdjToPair().
    */

    pairPos.clear();

    if (position == N || position >= seq.size()) return;

    st leftstPos = seq.prev(position);
    if(leftstPos != N)
    pairPos.push_back(leftstPos);
    st rightstPos = seq.next(position);
    if(rightstPos != N)
    pairPos.push_back(rightstPos);

}




void Repair::decreaseAdjToPair(st pos)
{
    vector<st> pairPos;
    findAdjPairs(pairPos, pos);

    for (auto i : pairPos)
    {
        st j = seq.next(i);
        if (j == N || j >= seq.size() || seq[i].code == N || seq[j].code == N)
            continue;

        auto key = make_pair(seq[i].code, seq[j].code);
        auto it = ht.find(key);
        if (it != ht.end())
        {
            PAIR* pair = it->second;

            pair->f_pos = i;

            q.removePair(pair);

            pair->freq--;

            if (pair->freq >= 1)
            {
                q.addPair(pair);
            }
            else
            {
                ht.erase(it);
                delete pair;
            }
        }
    }
}





void Repair::replace(st position)
{
    if (position >= seq.size() - 1) return;
    
    st actualNext = seq.next(position);
    if (actualNext == N || actualNext >= seq.size()) return;
    decreaseAdjToPair(position);
    
    st nextNext = seq.next(actualNext);
    
    seq[actualNext].code = static_cast<st>(N);
    seq[actualNext].prev = position;
    seq[actualNext].next = nextNext;
    

    st old_st = seq[position].code;
    seq[position].code = rule;

    st left_pos = seq.prev(position);
    if (left_pos != N && left_pos >= 0 && left_pos < seq.size() && seq[left_pos].code != N)
    {
        auto newPair1 = new PAIR{ seq[left_pos].code, rule };
        insert(newPair1, left_pos);
    }

    st right_pos = actualNext + 1;
    if (right_pos != N && right_pos < seq.size() && seq[right_pos].code != N)
    {
        auto newPair2 = new PAIR{ rule, seq[right_pos].code };
        insert(newPair2, position);
    }
}


void Repair::compactify()
{
    vector<SEQ> seq_new;

    for (st i = 0; i < seq.size(); i++) {
        if (seq[i].code != N) {
            seq_new.push_back(seq[i]);
        } 
        else if (seq[i].next < seq.size()) { 
            i = seq.next(i) - 1;
        }
    }

    st newStringSize = seq_new.size();
    st oldStringSize = seq.size();

    unordered_map<st, st> posMap;

    st oldPos = 0;
    st newPos = 0;

    while (oldPos < oldStringSize && newPos < newStringSize) {
        if (seq[oldPos].code != N) {
            posMap[oldPos] = newPos;
            newPos++;
        }
        oldPos++;
    }

    for (auto& pair_entry : ht) {
        PAIR* p = pair_entry.second;

        if (posMap.count(p->f_pos))
            p->f_pos = posMap[p->f_pos];

        if (posMap.count(p->b_pos))
            p->b_pos = posMap[p->b_pos];
    }

    for (st i = 0; i < seq_new.size(); i++) {
        if (seq_new[i].prev != N && posMap.count(seq_new[i].prev)) {
            seq_new[i].prev = posMap[seq_new[i].prev];
        } else {
            seq_new[i].prev = N;
        }

        if (seq_new[i].next != N && posMap.count(seq_new[i].next)) {
            seq_new[i].next = posMap[seq_new[i].next];
        } else {
            seq_new[i].next = N;
        }
    }

    seq = std::move(seq_new);
}


void Repair::compress(bool verbose)
{
    st nextRule = 256;
    vector<st> ruleHistory;
    st iterator = 0;
    st itert = 0;
    int threshold = static_cast<int>(ceil(seq.size() / 16.0)); //TODO: REMEMBER TO FIX THIS VALUE FFS
    while (true)
    {
       if(iterator % threshold == 0 && iterator > 0)
        {
            compactify();
        }
        int idx = q.size() - 1;
        while (idx >= 0 && q[idx]->next == nullptr)
        {
            idx--;
        }

        if (idx <= 1) break;
        PAIR* mostFreqPair = q[idx]->next->p;
        st rule = this->rule++;
        ruleHistory.push_back(mostFreqPair->left);
        ruleHistory.push_back(mostFreqPair->right);
        
        st pos = mostFreqPair->f_pos;

        pair<st, st> key = {mostFreqPair->left, mostFreqPair->right};

        st i = 0;
        while (i < seq.size() - 1)
        {
               
            st j = seq.next(i);
            if (j == N || j >= seq.size())
            {
                i++;
                continue;
            }
        
            if (seq[i].code == key.first && seq[j].code == key.second)
            {
                replace(i);
                //after replacing, i stays the same to catch overlaps like "aaa"
            }
            else
            {
                i = j;
            }
        }
        iterator++;

        q.removePair(mostFreqPair); 
        ht.erase(key);                 
        delete mostFreqPair;         
        if(verbose)
        {

            for (int i = 0; i < seq.size(); i++)
            {
                if(seq[i].code != N)
                {
                    if(seq[i].code < 128)
                    {
                        cout << static_cast<char>(seq[i].code);
                    }
                    else
                    {
                        cout << "["  << static_cast<int>(seq[i].code) << "]";
                    }
                }
            }
            itert++;
            cout << "\n" << "Iteration: " << itert << "\n";
            
            cout << "\nPress (ENTER) to continue to next iteration...\n";
            
            cin.get(); //wait for key press

        }
    }
    
    this->ruleHistory = ruleHistory;
}

