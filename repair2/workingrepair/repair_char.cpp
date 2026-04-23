
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


vector<PAIRNODE*> QCreate(string s)
{
    /*
    Always call this before firstPass. Initializes the pQueue.
    */
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
    /*

    Helper function. DO NOT CALL THIS EVER UNLESS AT THE END OF CODE!!!!
    
    */
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
    /*

    Helper function. DO NOT CALL THIS EVER UNLESS AT THE END OF CODE!!!!
    
    */
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
    printHashTable(ht);
    printBuckets(q);
    printConnections(s);
   
}

void insert(PAIR* p, unordered_map<pair<char, char>, PAIR*, PairHash>& htp, vector<PAIRNODE*>& heap, uint current_position, vector<SEQ>& s)
{
    /*
    Inserts a pair in the data structures. If this is the first time
    seeing a pair, add its ref to the hash map. If this is the second
    time you're seeing the pair, add its ref to the priority queue.
    Else, increase the value of its frequency by 1 and adjust its 
    position in the queue.
    */

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
        uint prev_pos = existing->b_pos;
    
        s[prev_pos].next = current_position;
        s[current_position].prev = prev_pos;
        
        QRemovePair(existing);

        existing->freq++;
        existing->b_pos = current_position;

        QAddPair(existing, heap);

        delete p;
    }
}


void firstPass(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<PAIRNODE*>& heap) 
{   
    /*
    Adds all pairs to the hash table, Priority queue
    and threads the SEQ vector.
    */

    uint string_size = s.size();
    
    unordered_map<pair<char, char>, uint, PairHash> last_occurrence;
    
    for (uint i = 0; i < string_size - 1; i++) 
    {
        pair<char, char> current_pair = {s[i].code, s[i+1].code};
        
        PAIR* new_pair = new PAIR;
        new_pair->left = current_pair.first;
        new_pair->right = current_pair.second;
        insert(new_pair, ht, heap, i, s);
        
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

    //if its the last element, it does not have a pair (just for consistency)
    if (!s.empty())
    {
        s[string_size - 1].prev = N;
        s[string_size - 1].next = N;
    }
}

uint next(vector<SEQ>& s, uint pos) {
    if (pos >= s.size()) return N;
    uint current = pos + 1;
    while (current < s.size() && s[current].code == N)
    {
        if (s[current].next == N) return N;
        current = s[current].next;
    }

    if (pos + 1 < s.size() && s[pos + 1].code == N) {
        s[pos + 1].next = current;
    }
    return current;
}

uint prev(const vector<SEQ>& s, uint pos)
{
    if (pos == 0) return N;
    
    uint current = pos - 1;
    
    while (current < s.size() && s[current].code == N)
    {
        if (s[current].prev == N) return N;
    
        current = s[current].prev;
    }
    
    return current;
}

void findAdjPairs(unordered_map<pair<char, char>, PAIR*, PairHash>& ht, vector<SEQ>& s, vector<uint>& pairPos, uint position)
{
    /*
    Locates adjacent pairs relative to position.
    Helper function for decreaseAdjToPair().
    */
    pairPos.clear();

    if (position == N || position >= s.size()) return;

    uint leftCharPos = prev(s, position);
    if(leftCharPos != N)
    pairPos.push_back(leftCharPos);
    uint rightCharPos = next(s, position);
    if(rightCharPos != N)
    pairPos.push_back(rightCharPos);

}




void decreaseAdjToPair(
    unordered_map<pair<char, char>, PAIR*, PairHash>& ht,
    vector<SEQ>& s,
    vector<PAIRNODE*>& heap,
    uint pos)
{
    vector<uint> pairPos;
    findAdjPairs(ht, s, pairPos, pos);

    for (uint i : pairPos)
    {
       // cout << "PairPos: "<<i <<"\n";
        uint j = ::next(s, i);
        if (j == N || j >= s.size() || s[i].code == N || s[j].code == N)
            continue;

        auto key = make_pair(s[i].code, s[j].code);
        auto it = ht.find(key);
        if (it != ht.end())
        {
            PAIR* pair = it->second;

            pair->f_pos = i;

            QRemovePair(pair);

            pair->freq--;

            if (pair->freq >= 1)
            {
                QAddPair(pair, heap);
            }
            else
            {
                ht.erase(it);
                delete pair;
            }
        }
    }
}





void replace(
    vector<SEQ>& s, 
    uint position, 
    char rule, 
    unordered_map<pair<char, char>, PAIR*, PairHash>& ht, 
    vector<PAIRNODE*>& q)
{
    if (position >= s.size() - 1) return;
    
    uint actualNext = next(s, position);
    if (actualNext == N || actualNext >= s.size()) return;
    decreaseAdjToPair(ht, s, q, position);
    
    uint nextNext = next(s, actualNext);
    
    s[actualNext].code = static_cast<char>(N);
    s[actualNext].prev = position;
    s[actualNext].next = nextNext;
    

    char old_char = s[position].code;
    s[position].code = rule;

    uint left_pos = prev(s, position);
    if (left_pos != N && left_pos >= 0 && left_pos < s.size() && s[left_pos].code != N)
    {
        auto newPair1 = new PAIR{ s[left_pos].code, rule };
        insert(newPair1, ht, q, left_pos, s);
    }

    uint right_pos = actualNext + 1;
    if (right_pos != N && right_pos < s.size() && s[right_pos].code != N)
    {
        auto newPair2 = new PAIR{ rule, s[right_pos].code };
        insert(newPair2, ht, q, position, s);
    }
}


void compactify(unordered_map<pair<char, char>, PAIR*, PairHash>& ht,
vector<SEQ>& s,
vector<PAIRNODE*>& q)
{
        
    //1: reconstruct the sequence:

    vector<SEQ> seq;

    for (int i = 0; i < s.size(); i++) {
        if (s[i].code != N)
        {
            seq.push_back(s[i]);
        } 
        else if (s[i].next < s.size()) 
        { 
            i = next(s, i) - 1;
        }
    }


    int newStringSize = seq.size();
    int oldStringSize = s.size();

    //2: create map oldpos to newpos:

    unordered_map<uint, uint> posMap;

    uint oldPos = 0;
    uint newPos = 0;

    while (oldPos < oldStringSize && newPos < newStringSize)
    {
        if (s[oldPos].code != N)
        {
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
         s[p->b_pos].code = static_cast<char>(N);
    }

    for (uint i = 0; i < seq.size(); i++) {
        if (seq[i].prev != N && posMap.count(seq[i].prev)) {
            seq[i].prev = posMap[seq[i].prev];
        } else {
            seq[i].prev = N;
        }
        
        if (seq[i].next != N && posMap.count(seq[i].next)) {
            seq[i].next = posMap[seq[i].next];
        } else {
            seq[i].next = N;
        }
    }
    s = seq;


}


void repair(
    unordered_map<pair<char, char>, PAIR*, PairHash>& ht,
    vector<SEQ>& s,
    vector<PAIRNODE*>& q, string& history)
{
    char nextRule = 65;
    string ruleHistory;
    uint iterator = 0;
    uint itert = 0;
    int threshold = static_cast<int>(ceil(s.size() / 16.0)); //TODO: REMEMBER TO FIX THIS VALUE FFS
    while (true)
    {
       // cout << "Compactify Count: " << iterator << " (threshold: " << threshold << ")\n";
        if(iterator % threshold == 0 && iterator > 0)
        {
            compactify(ht, s, q);
        }
        int idx = q.size() - 1;
        while (idx >= 0 && q[idx]->next == nullptr)
        {
            idx--;
        }

        if (idx <= 1) break;
        //printDS(ht, s, q);
        PAIR* mostFreqPair = q[idx]->next->p;
        char rule = nextRule++;
        /*
        cout << "\nReplacing most frequent pair: ("
        << mostFreqPair->left << ", " << mostFreqPair->right
        << ") with '" << rule << "' (frequency: " << mostFreqPair->freq << ")\n";
        */
        ruleHistory.push_back(mostFreqPair->left);
        ruleHistory.push_back(mostFreqPair->right);
        
        uint pos = mostFreqPair->f_pos;

        pair<char, char> key = {mostFreqPair->left, mostFreqPair->right};

        uint i = 0;
        while (i < s.size() - 1)
        {
               
            uint j = next(s, i);
            if (j == N || j >= s.size())
            {
                i++;
                continue;
            }
        
            if (s[i].code == key.first && s[j].code == key.second)
            {
                
                replace(s, i, rule, ht, q);
                //after replacing, i stays the same to catch overlaps like "aaa"
            }
            else
            {
                i = j;
            }
        }
        iterator++;

        QRemovePair(mostFreqPair); 
        ht.erase(key);                 
        delete mostFreqPair;         
        
       // cout << "\n--- AFTER REPLACEMENT ---\n";
       // printDS(ht, s, q);
        for (int i = 0; i < s.size(); i++)
        {
            if(s[i].code != N)
            {
                if(s[i].code < 128)
                {
                    cout << s[i].code;
                }
                else
                {
                    cout << "["  << static_cast<int>(s[i].code) << "]";
                }
            }
        }
        itert++;
        cout << "\n" << "Iteration: " << itert << "\n";
        cout << "\nPress (ENTER) to continue to next iteration...\n";
        
        cin.get(); // Wait for key press
    }
    cout << "\nRules: " << ruleHistory << "\n";
    history = ruleHistory;
}





int main()
{
    string input = "singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do singing do wah diddy diddy dum diddy do ";
    // string input = "abababcdefghijklmnopqrstuvwxyzcdefghijklmnopqrstuvwxyzcdefghijklmnopqrstuvwxyzcdefghijklmnopqrstuvwxyz";
    
    auto q = QCreate(input);
    vector<SEQ> s = initializeSequence(input);
    unordered_map<pair<char, char>, PAIR*, PairHash> ht;
    string history;
    string cfg;
    firstPass(ht, s, q);

    int size = s.size();
    repair(ht, s, q, history);

    cout << "\n";
    for (int i = 0; i < s.size(); i++)
    {
        if(s[i].code != N)
        {
            cfg.push_back(s[i].code);
        }
    }
    cout << "CFG: " << cfg << "\n";
    cout << "\n";
    cout << "Size of input string: " << size << "\n";
    freeAll(ht, q);
    return 0;
}
