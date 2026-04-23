#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <climits>
typedef unsigned int uint;
using namespace std;
#define N UINT_MAX

struct PAIR
{
    char left;
    char right;
    mutable uint freq = 1;
    uint f_pos = N; //ending pos
    uint b_pos = N; //beginning pos
    bool operator==(const PAIR& other) const
    {
        return left == other.left && right == other.right;
    }

    bool operator<(const PAIR& other) const
    {
        return freq < other.freq;
    }
};

struct PairHash
{
    size_t operator()(const pair<char, char>& p) const
    {
        return hash<char>()(p.first) ^ (hash<char>()(p.second) << 1);
    }
};

struct SEQ
{
    char code = 0;
    uint next = N;
    uint prev = N;
};

vector<SEQ> initializeSequence(const string& s)
{
    uint string_size = s.size();
    vector<SEQ> sequence(string_size);
    for (uint i = 0; i < string_size; i++)
    {
        sequence[i].code = s[i];
    }
    return sequence;
}

void insert(const PAIR& p, unordered_map<pair<char, char>, PAIR, PairHash>& htp, vector<PAIR*>& heap) {
    auto key = make_pair(p.left, p.right);
    auto it = htp.find(key);

    if (it == htp.end())
    {
        htp.insert({key, p});
    } 
    else
    {
        it->second.freq++;

        if (it->second.freq == 2)
        {
            heap.push_back(&it->second);
        }
    }
}

void printHeap(const vector<PAIR*>& heap)
{
    for (const auto& p : heap)
    {
        cout << "(" << p->left << ", " << p->right << ") -> freq: " << p->freq << endl;
    }
}

void printHashTable(const unordered_map<pair<char, char>, PAIR, PairHash>& ht)
{
    for (const auto& entry : ht) {
        cout << "(" << entry.first.first << ", " << entry.first.second << ") "
             << "-> freq: " << entry.second.freq
             << ", f_pos: " << (entry.second.f_pos == N ? "NULL" : to_string(entry.second.f_pos))
             << ", b_pos: " << (entry.second.b_pos == N ? "NULL" : to_string(entry.second.b_pos))
             << endl;
    }
}
void makeHeap(vector<PAIR*>& heap)
{
    make_heap(heap.begin(), heap.end(), [](const PAIR* a, const PAIR* b)
    {
        return *a < *b;
    });
}

void printConnections(const vector<SEQ>& seq) {
    for (size_t i = 0; i < seq.size(); i++) {
        auto printIndex = [](uint idx) -> string {
            return (idx == N) ? "NULL" : to_string(idx);
        };
        
        cout << "SEQ " << i << " (code: " << seq[i].code << ")";
        
        // Check if the code is N and adjust the output accordingly
        if (seq[i].code == N) {
            cout << " -> Thread: " << printIndex(seq[i].next); // Use 'Thread:' if code is N
        } else {
            cout << " -> Next: " << printIndex(seq[i].next); // Normal behavior for code != N
        }
        if(seq[i].code == N)
        {
            cout << ", Prev. Thread: " << printIndex(seq[i].prev) << "\n"; 
        }
        else
        {
            cout << ", Prev: " << printIndex(seq[i].prev) << "\n";
        }
    }
}


void countPairs(unordered_map<pair<char, char>, PAIR, PairHash>& ht, const vector<SEQ>& s, vector<PAIR*>& heap) 
{
    uint string_size = s.size();
    for (uint i = 0; i < string_size - 1; i++) 
    {
        PAIR new_pair;
        new_pair.left = s[i].code;
        new_pair.right = s[i + 1].code;
        insert(new_pair, ht, heap);
    }
}

void linkArray(unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s) 
{
    for (size_t i = 0; i < s.size() - 1; i++)
    {
        auto key = make_pair(s[i].code, s[i + 1].code);
        auto it = ht.find(key);

        if (it != ht.end())
        {
            PAIR& pair = it->second;

            if (pair.f_pos == N) 
            {
                pair.f_pos = static_cast<uint>(i);
                pair.b_pos = static_cast<uint>(i);
            }
            else
            {
                s[pair.f_pos].next = static_cast<uint>(i);
                s[i].prev = pair.f_pos;
                pair.f_pos = static_cast<uint>(i);

            }
        }
    }
}

void locateAdjacentPairs(const PAIR& p, unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<uint>& pairPos)
{
    auto key = make_pair(p.left, p.right);
    auto it = ht.find(key);
    pairPos.clear();

    if(it != ht.end())
    {
        uint curr = it->second.b_pos;
        while (curr != N && curr < s.size() - 1)
        {
            if(curr > 0)
            {
                if(s[curr - 1].code != N)
                {
                    pairPos.push_back(curr - 1);
                }
                else
                {
                    pairPos.push_back(s[curr - 1].prev);
                }
            }
            if(curr  + 1 < s.size())
            {
                if (s[curr + 1].code != N)
                {
                    pairPos.push_back(curr + 1);
                }
                else
                {
                    pairPos.push_back(s[curr + 1].next);
                }
            }

            curr = s[curr].next;
        }
        
    }
    return;
}

void decreaseAdjacents(vector<PAIR*>& heap, unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<uint>& pairPos)
{
    for (uint i = 0; i < pairPos.size(); i++)
    {
        uint left = s[pairPos[i]].code;
        uint right;

        if (pairPos[i] + 1 < s.size()) {
            if (s[pairPos[i] + 1].code == N)
            {
                right = s[s[pairPos[i] + 1].next].code;
            }
            else
            {
                right = s[pairPos[i] + 1].code;
            }
        } else {
            continue;
        }

        auto key = make_pair(left, right);
        auto it = ht.find(key);
        if (it != ht.end())
        {
            PAIR& pair = it->second;
            pair.freq--;

            if (pair.freq == 1)
            {
                auto pos = find(heap.begin(), heap.end(), &pair);
                if (pos != heap.end())
                {
                    heap.erase(pos);
                    make_heap(heap.begin(), heap.end(), [](const PAIR* a, const PAIR* b)
                    {
                        return *a < *b;
                    });
                }
            }

            if (pair.freq == 0)
            {
                ht.erase(it);
                auto pos = find(heap.begin(), heap.end(), &pair);
                if (pos != heap.end())
                {
                    heap.erase(pos);
                    make_heap(heap.begin(), heap.end(), [](const PAIR* a, const PAIR* b)
                    {
                        return *a < *b;
                    });
                }
            }
        }
    }
}



void replacePair(const PAIR& p, unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s,  char value)
{
    auto key = make_pair(p.left, p.right);
    auto it = ht.find(key);

    if(it != ht.end())
    {
        uint curr = it->second.b_pos;

        while(curr != N && curr < s.size() - 1)
        {
            s[curr].code = value;
            
            if(s[curr + 1].code == N || (curr + 2) <= s.size())
            {
                s[curr + 1].next = curr + 2;
                s[curr + 1].prev = curr;
            }
            
            s[curr + 1].code = static_cast<char>(N);
            curr = s[curr].next;
        }
        
    }

}

void increaseNewPairs(const PAIR& p, unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<PAIR*>& heap)
{
        auto key = make_pair(p.left, p.right);
        auto it = ht.find(key);
        if (it == ht.end()) return;  
    
        uint curr = it->second.b_pos;
        uint b_pos_next = curr;
    
        uint b_pos_prev = 2;
    
    
    /*
    if(curr + 1 > 0 && s[curr + 1].code == N)
    {
        b_pos_prev = s[curr + 1].prev;
    }
    else
    {
        b_pos_prev = curr - 1;
    }
    */
        while (curr != N && curr < s.size() - 1)
        {
            if (curr + 2 < s.size())
            {
                char leftNext = s[curr].code;
                char rightNext;
                if (s[curr + 1].code == N)
                {
                    if(s[curr + 1].next < s.size() && s[s[curr + 1].next].code == N)
                    {
                        s[curr + 1].next = s[s[curr + 1].next].next;
                    }
                    rightNext = s[s[curr + 1].next].code;
                }
                else
                {
                    rightNext = s[curr + 1].code;
                }    
                PAIR pNext;
                pNext.left = leftNext;
                pNext.right = rightNext;
                pNext.b_pos = b_pos_next;
                insert(pNext, ht, heap);
            }
            if(curr > 0)
            {
                char rightPrev = s[curr].code;
                char leftPrev;
                if(s[curr - 1].code == N)
                {
                    if(s[curr - 1].next > 0 && s[s[curr - 1].next].code == N)
                    {
                        s[curr + 1].next = s[s[curr + 1].next].next;
                    }
                    leftPrev = s[s[curr - 1].prev].code;
                }
                else
                {
                    leftPrev = s[curr - 1].code;
                }
                PAIR pPrev;
                pPrev.left = leftPrev;
                pPrev.right = rightPrev;
                pPrev.b_pos = b_pos_prev;
                insert(pPrev, ht, heap); 
    
            }
    
    
            curr = s[curr].next;    
        }
    
    }



void repairAllPairs(unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<PAIR*>& heap) {

    char new_char = 'X';
    //while (!heap.empty()) {
        
        printHeap(heap);
        cout << "\n";
        printHashTable(ht);
        cout << "\n";
        printConnections(s);
        cout << "\n";


        PAIR* current_pair = heap.front();
        pop_heap(heap.begin(), heap.end(), [](const PAIR* a, const PAIR* b) { return *a < *b; });
        heap.pop_back();


        vector<uint> adjacentPairPos;
        locateAdjacentPairs(*current_pair, ht, s, adjacentPairPos);

        decreaseAdjacents(heap, ht, s, adjacentPairPos);


        replacePair(*current_pair, ht, s, new_char);
        new_char += 1;

        increaseNewPairs(*current_pair, ht, s, heap);

        auto key = make_pair(current_pair->left, current_pair->right);
        auto it = ht.find(key);
        if (it != ht.end() && it->second.freq == 0) {
            ht.erase(it);
        }

        make_heap(heap.begin(), heap.end(), [](const PAIR* a, const PAIR* b) { return *a < *b; });



    //}
}



int main() {
    string String = "abcdabcd";


    vector<PAIR*> heap{};
    vector<uint> adjacentPairPos;
    heap.reserve(sqrt(String.size()));

    unordered_map<pair<char, char>, PAIR, PairHash> hashTable;
    vector<SEQ> seq = initializeSequence(String);

    countPairs(hashTable, seq, heap);

    linkArray(hashTable, seq);

    makeHeap(heap);

    cout << "Initial Heap:" << endl;
    printHeap(heap);
    cout << "\nInitial Hash Table:" << endl;
    printHashTable(hashTable);
    cout << "\nInitial Sequence Connections:" << endl;
    printConnections(seq);

    repairAllPairs(hashTable, seq, heap);

    cout << "\nAfter Repairing All Pairs:" << endl;
    printHeap(heap);
    cout << "\nHash Table After Repair:" << endl;
    printHashTable(hashTable);
    cout << "\nSequence Connections After Repair:" << endl;
    printConnections(seq);

    return 0;
}