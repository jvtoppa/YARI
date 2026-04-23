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
        
        if (seq[i].code == N) {
            cout << " -> Thread: " << printIndex(seq[i].next); 
        } else {
            cout << " -> Next: " << printIndex(seq[i].next);
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

        while(curr < s.size() - 1)
        {
            s[curr].code = value;
            
            if(s[curr + 1].code == N && s[curr + 1].next < s.size() - 1)
            {
                s[s[curr + 1].next].code = static_cast<char>(N);
                s[s[curr + 1].next].prev = s[curr + 1].prev;
                s[s[curr + 1].next].next = s[curr + 1].next + 2;
            }
            else
            {
                s[curr + 1].code = static_cast<char>(N);
                s[curr + 1].prev = curr;
                s[curr + 1].next = curr + 2;

            }
            
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

    uint b_pos_prev;

    if(curr - 1 > 0)
    {
        b_pos_prev = curr - 1;
    }
    else
    {
        b_pos_prev = N;
    }

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
            cout << "Increasing Pair " << pNext.left << ", " << pNext.right << "\n";
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
            cout << "Increasing Pair " << pPrev.left << ", " << pPrev.right << "\n";
            
            insert(pPrev, ht, heap); 

        }


        curr = s[curr].next;    
    }

}

void compactify(unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<PAIR*>& heap)
{
    vector<SEQ> result;
    for (int i = 0; i < s.size(); i++)
    {
        if(s[i].code != N)
        {
            s[i].next = N;
            s[i].prev = N;
            result.push_back(s[i]);
        }
    }
    s = result;
    ht.clear();
    heap.clear();
    countPairs(ht, s, heap);
    linkArray(ht, s);
    makeHeap(heap);
}

void printCurrentString(const vector<SEQ>& seq)
{

    for (int i = 0; i < seq.size(); i++)
    {
        if(seq[i].code != N)
        {
            cout << seq[i].code;
        }
    }
    cout << "\n";
}

void repairAllPairs(unordered_map<pair<char, char>, PAIR, PairHash>& ht, vector<SEQ>& s, vector<PAIR*>& heap) {
    char new_char = 'X';
    uint iter = 0;
    uint iterator = 0;
    while (!heap.empty()) {
        iter++;

        cout << "Iteration " << iter << "\n";
        cout << "Heap: \n";
        printHeap(heap);
        cout << "\nHash: \n";
        printHashTable(ht);
        cout << "\n";
        printConnections(s);
        cout << "\n";
        printCurrentString(s);

        PAIR* current_pair = heap.front();
        cout << "Active pair: " << current_pair->left << ", " << current_pair->right << "\n";
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
        if (it != ht.end()) {
            ht.erase(it);
        }
        iterator++;
        if(iterator % static_cast<int>(ceil(s.size() / 4.0)) == 0 && iterator > 0)
        {
            iterator = 0;
            compactify(ht, s, heap);
        }
    
    }
}


int main() {
    string String = "abcdabcdabcd";


    vector<PAIR*> heap{};
    vector<uint> adjacentPairPos;
    heap.reserve(sqrt(String.size()));

    unordered_map<pair<char, char>, PAIR, PairHash> hashTable;
    vector<SEQ> seq = initializeSequence(String);

    countPairs(hashTable, seq, heap);

    linkArray(hashTable, seq);

    makeHeap(heap);


    repairAllPairs(hashTable, seq, heap);
    printConnections(seq);

    printHeap(heap);
    cout << "\n";
    printHashTable(hashTable);
    return 0;
}