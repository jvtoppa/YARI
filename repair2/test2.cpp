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
}SEQ;

struct PAIR
{
    char left;
    char right;
    mutable uint freq = 1;
    uint f_pos = N; //beginning pos
    uint b_pos = N; //ending pos
    struct PAIR* next;
    struct PAIRNODE* node = nullptr;
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
    PAIRNODE* prev;
};

struct PairHash
{
    size_t operator()(const pair<char, char>& p) const
    {
        return hash<char>()(p.first) ^ (hash<char>()(p.second) << 1);
    }
};

// Helper function to find next valid position after tombstones
uint findNextValid(const vector<SEQ>& s, uint pos) {
    while (pos < s.size() && s[pos].code == N) {
        pos = s[pos].next; // Follow tombstone's next pointer
    }
    return pos;
}

// Helper function to find previous valid position before tombstones
uint findPrevValid(const vector<SEQ>& s, uint pos) {
    while (pos < s.size() && s[pos].code == N) {
        pos = s[pos].prev; // Follow tombstone's prev pointer
    }
    return pos;
}

vector<PAIRNODE*> QCreate(string s)
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

void QAddPair(PAIR* p, vector<PAIRNODE*>& q)
{
    uint pos = p->freq;
    if (pos >= q.size()) pos = q.size() - 1;

    PAIRNODE* head = q[pos];

    PAIRNODE* newNode = new PAIRNODE();
    newNode->p = p;
    
    newNode->next = head->next;
    newNode->prev = head;
    
    if (head->next)
        head->next->prev = newNode;
    
    head->next = newNode;
    p->node = newNode;
}

void QRemovePair(PAIR* p)
{
    if (!p || !p->node) return;

    PAIRNODE* node = p->node;

    if (node->prev)
        node->prev->next = node->next;

    if (node->next)
        node->next->prev = node->prev;

    p->node = nullptr;
    delete node;
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
            delete temp;
        }
    }
}

void freeHashTable(unordered_map<pair<char, char>, PAIR*, PairHash>& ht)
{
    for (auto& kv : ht)
    {
        delete kv.second;
    }
    ht.clear();
}

void freeAll(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<PAIRNODE*>& q)
{
    freeHashTable(ht);
    freeBuckets(q);
}

void printBuckets(const vector<PAIRNODE*>& q)
{
    for (int i = 0; i < q.size(); ++i)
    {
        cout << "Bucket[" << i << "]: ";
        PAIRNODE* curr = q[i]->next;
        while (curr)
        {
            cout << "(" << curr->p->left << curr->p->right << ":" << curr->p->freq << ") ";
            curr = curr->next;
        }
        cout << endl;
    }
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

void printDS(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<PAIRNODE*>& q)
{
    printBuckets(q);
    printConnections(s);
    printHashTable(ht); // Commented out for clarity
}
vector<SEQ> initializeSequence(const string& s)
{
    /*
    Always call this before first pass. 
    Initializes the SEQ vector.
    */
    uint string_size = s.size();
    vector<SEQ> sequence(string_size);
    for (uint i = 0; i < string_size; i++)
    {
        sequence[i].code = s[i];
    }
    return sequence;
}

void insert(PAIR* p, unordered_map<pair<char, char>, PAIR*, PairHash>& htp, vector<PAIRNODE*>& heap, uint current_position)
{
    auto key = make_pair(p->left, p->right);
    auto it = htp.find(key);

    if (it == htp.end())
    {
        p->f_pos = current_position;
        p->b_pos = current_position;
        htp[key] = p;
    }
    else
    {
        PAIR* existing = it->second;

        QRemovePair(existing);

        existing->freq++;
        existing->b_pos = current_position;

        QAddPair(existing, heap);

        delete p;
    }
}

void firstPass(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<PAIRNODE*>& heap) 
{   
    uint string_size = s.size();
    
    unordered_map<pair<char, char>, uint, PairHash> last_occurrence;
    
    for (uint i = 0; i < string_size - 1; i++) 
    {
        pair<char, char> current_pair = {s[i].code, s[i+1].code};
        
        PAIR* new_pair = new PAIR;
        new_pair->left = current_pair.first;
        new_pair->right = current_pair.second;
        insert(new_pair, ht, heap, i);
        
        if (last_occurrence.find(current_pair) != last_occurrence.end())
        {
            uint prev_pos = last_occurrence[current_pair];
            s[i].prev = prev_pos;            
            s[prev_pos].next = i;            
        }
        else
        {
            s[i].prev = N;
        }
        s[i].next = N;
        last_occurrence[current_pair] = i;
    }

    // Set up initial threading for consecutive same pairs
    if (!s.empty())
    {
        s[string_size - 1].prev = N;
        s[string_size - 1].next = N;
    }
}

void findAdjPairs(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<uint>& pairPos, uint position)
{
    pairPos.clear();

    if (position == N || position >= s.size()) return;

    // Left adjacent - find previous valid character
    if (position > 0) {
        uint left_pos = position - 1;
        if (left_pos < s.size() && s[left_pos].code != N) {
            pairPos.push_back(left_pos);
        }
    }

    // Right adjacent - find next valid character
    if (position + 1 < s.size() && s[position + 1].code != N) {
        pairPos.push_back(position);
    }
}

void decreaseAdjToPair(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<PAIRNODE*>& heap, uint position)
{
    vector<uint> pairPos;
    findAdjPairs(ht, s, pairPos, position);

    for (uint i : pairPos)
    {
        if (i + 1 >= s.size()) continue;

        auto key = make_pair(s[i].code, s[i + 1].code);
        auto it = ht.find(key);

        if (it != ht.end())
        {
            PAIR* pair = it->second;

            QRemovePair(pair);

            if (pair->freq > 1) {
                pair->freq--;
                if (pair->freq > 1) {
                    QAddPair(pair, heap);
                } else {
                    ht.erase(it);
                    delete pair;
                }
            } else {
                ht.erase(it);
                delete pair;
            }
        }
    }
}

// Helper function to find the actual next valid character position
uint findActualNext(const vector<SEQ>& s, uint pos) {
    if (pos >= s.size()) return N;
    uint current = pos + 1;
    while (current < s.size() && s[current].code == N) {
        if (s[current].next == N) return N;
        current = s[current].next;
    }
    return current;
}

// Helper function to find the actual previous valid character position  
uint findActualPrev(const vector<SEQ>& s, uint pos) {
    if (pos == 0) return N;
    uint current = pos - 1;
    while (current < s.size() && s[current].code == N) {
        if (s[current].prev == N) return N;
        current = s[current].prev;
    }
    return current;
}

void replace(
    vector<SEQ>& s, 
    uint position, 
    char rule, 
    unordered_map<pair<char, char>, PAIR*, PairHash>& ht, 
    vector<PAIRNODE*>& q)
{
    if (position >= s.size() - 1) return;
    
    // Find actual next character (skip tombstones)
    uint actual_next = findActualNext(s, position);
    if (actual_next == N || actual_next >= s.size()) return;
    
    // Store what comes after the next character
    uint after_next = s[actual_next].next;
    
    // Mark actual_next as tombstone
    s[actual_next].code = N;
    s[actual_next].prev = position;
    
    // Tombstone threading: point to next valid character
    uint temp = actual_next + 1;
    while (temp < s.size() && temp != actual_next) { // avoid infinite loop
        if (s[temp].code != N) {
            s[actual_next].next = temp;
            break;
        }
        temp++;
        if (temp <= actual_next) break; // safety check
    }
    if (temp >= s.size() || temp == actual_next) {
        s[actual_next].next = after_next;
    }
    
    // Update current position
    char old_char = s[position].code;
    s[position].code = rule;
    
    // Update links
    s[position].next = after_next;
    if (after_next != N && after_next < s.size()) {
        s[after_next].prev = position;
    }
    
    // Update adjacent pairs
    decreaseAdjToPair(ht, s, q, position);

    // Add new pairs created by the replacement
    uint left_pos = findActualPrev(s, position);
    if (left_pos != N && left_pos < s.size() && s[left_pos].code != N) {
        auto newPair1 = new PAIR{ s[left_pos].code, rule };
        insert(newPair1, ht, q, left_pos);
    }

    uint right_pos = s[position].next;
    if (right_pos != N && right_pos < s.size() && s[right_pos].code != N) {
        auto newPair2 = new PAIR{ rule, s[right_pos].code };
        insert(newPair2, ht, q, position);
    }
}

int main()
{
    string input = "abcabcabc";
    auto q = QCreate(input);
    vector<SEQ> s = initializeSequence(input);
    unordered_map<pair<char, char>, PAIR*, PairHash> ht;

    firstPass(ht, s, q);

    cout << "\n--- BEFORE REPLACE ---\n";
    printDS(ht, s, q);

    replace(s, 0, 'X', ht, q);
    replace(s, 0, 'Y', ht, q);
    replace(s, 0, 'Z', ht, q);
    cout << "\n--- AFTER REPLACE (position 0 -> 'ab' replaced with 'X') ---\n";
    printDS(ht, s, q);

    freeAll(ht, q);
    return 0;
}