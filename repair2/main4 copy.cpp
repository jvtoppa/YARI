
#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#define N UINT_MAX
typedef unsigned int uint;
using namespace std;
typedef struct sequence
{
    char code;
    uint prev;
    uint next;
}SEQ ;

struct PAIR
{
    char left;
    char right;
    mutable uint freq = 1;
    uint f_pos = N; //beginning pos
    uint b_pos = N; //ending pos
    struct PAIR* next;
    bool operator==(const PAIR& other) const
    {
        return left == other.left && right == other.right;
    }

    bool operator<(const PAIR& other) const
    {
        return freq < other.freq;
    }
};

struct PAIRNODE
{
PAIR* p;
PAIRNODE* next;

};

struct PairHash
{
    size_t operator()(const pair<char, char>& p) const
    {
        return hash<char>()(p.first) ^ (hash<char>()(p.second) << 1);
    }
};


vector<PAIRNODE*> createQ(string s)
{
    vector<PAIRNODE*> q(static_cast<size_t>(ceil(sqrt(s.size())) + 2));

    for (int i = 0; i < q.size(); i++)
    {
        PAIRNODE* p = new PAIRNODE();
        p->next = nullptr;
        p->p = nullptr;
        q[i] = p;
    }

    return q;   
}


void addPairHeap(PAIR* p, vector<PAIRNODE*>& q)
{
    uint pos = p->freq;

    if (pos >= q.size())
    {
        pos = q.size() - 1;

        PAIRNODE* head = q[pos];  // Dummy head node
        PAIRNODE* prev = head;
        PAIRNODE* curr = head->next;

        while (curr && curr->p->freq > p->freq)
        {
            prev = curr;
            curr = curr->next;
        }

        PAIRNODE* newNode = new PAIRNODE();
        newNode->p = p;
        newNode->next = curr;
        prev->next = newNode;
    }
    else
    {
        PAIRNODE* head = q[pos];
        PAIRNODE* newNode = new PAIRNODE();
        newNode->p = p;
        newNode->next = head->next;
        head->next = newNode;
    }
}

void freeBuckets(vector<PAIRNODE*>& q)
{
    for (auto& head : q)
    {
        PAIRNODE* curr = head;
        while (curr)
        {
            PAIRNODE* temp = curr;
            curr = curr->next;
            delete temp->p;
            delete temp;
        }
    }
}


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

void printHashTable(const unordered_map<pair<char, char>, PAIR*, PairHash>& ht)
{
    for (const auto& entry : ht) {
        cout << "(" << entry.first.first << ", " << entry.first.second << ") "
             << "-> freq: " << entry.second->freq
             << ", f_pos: " << (entry.second->f_pos == N ? "NULL" : to_string(entry.second->f_pos))
             << ", b_pos: " << (entry.second->b_pos == N ? "NULL" : to_string(entry.second->b_pos))
             << endl;
    }
}

void insert(PAIR* p, unordered_map<pair<char, char>, PAIR*, PairHash>& htp, vector<PAIRNODE*>& heap, uint current_position)
{
    auto key = make_pair(p->left, p->right);
    auto it = htp.find(key);
    if (it == htp.end())
    {
        p->f_pos = current_position;
        p->b_pos = current_position;
        htp.insert({key, p});
    } 
    else
    {
        it->second->freq++;
        it->second->b_pos = current_position;
        if (it->second->freq == 2)
        {
            addPairHeap(it->second, heap);
        }
    }
}

void countPairs(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<PAIRNODE*>& heap) 
{
    uint string_size = s.size();
    
    // Map to track the last position of each pair for linking prev/next in SEQ array
    unordered_map<pair<char, char>, uint, PairHash> last_occurrence;
    
    for (uint i = 0; i < string_size - 1; i++) 
    {
        pair<char, char> current_pair = {s[i].code, s[i+1].code};
        
        // Create the new pair object and insert into hash map / heap as before
        PAIR* new_pair = new PAIR;
        new_pair->left = current_pair.first;
        new_pair->right = current_pair.second;
        insert(new_pair, ht, heap, i);
        
        // Link prev/next pointers in SEQ based on last occurrence of this pair
        if (last_occurrence.find(current_pair) != last_occurrence.end())
        {
            uint prev_pos = last_occurrence[current_pair];
            s[i].prev = prev_pos;            // current's prev points to last occurrence
            s[prev_pos].next = i;            // last occurrence's next points to current
        }
        else
        {
            // No previous occurrence, so prev is NULL
            s[i].prev = N;
        }
        
        // Initially, next is NULL (updated when a later occurrence is found)
        s[i].next = N;
        
        // Update last occurrence to current position
        last_occurrence[current_pair] = i;
    }
    
    // The last element (string_size-1) doesn't form a pair, set prev and next to N
    if (!s.empty())
    {
        s[string_size - 1].prev = N;
        s[string_size - 1].next = N;
    }
}


int main()
{
    string input = "abcdefghijklmno";
    auto q = createQ(input);
    vector<SEQ> sequence = initializeSequence(input);

    vector<PAIR*> testPairs = {
        new PAIR{'A', 'B', 2},
        new PAIR{'C', 'D', 3},
        new PAIR{'E', 'F', 7},
        new PAIR{'G', 'H', 9},
        new PAIR{'I', 'J', 9},
        new PAIR{'K', 'L', 9},
        new PAIR{'M', 'N', 12},
        new PAIR{'O', 'P', 14},
        new PAIR{'Q', 'R', 20},
        new PAIR{'S', 'T', 18},
        new PAIR{'U', 'V', 15},
        new PAIR{'W', 'X', 25}
    };

    for (auto* p : testPairs)
    {
        addPairHeap(p, q);
    }

    freeBuckets(q);

    return 0;
}