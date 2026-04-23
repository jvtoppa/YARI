#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

using Pair = pair<char, char>;

struct PairHash {
    size_t operator()(const Pair& p) const {
        return hash<char>()(p.first) ^ (hash<char>()(p.second) << 1);
    }
};

struct Node {
    char symbol;
    Node* prev = nullptr;
    Node* next = nullptr;
    Node(char sym) : symbol(sym) {}
};

class RepairCompressor {
    Node* head = nullptr;
    Node* tail = nullptr;

    unordered_map<Pair, list<Node*>, PairHash> pairPositions;
    unordered_map<Pair, int, PairHash> pairFreq;
    vector<list<Pair>> freqBuckets;
    unordered_set<Pair, PairHash> replacedPairs;
    int maxFreq = 0;
    char nextSymbol = 127;

    void link(Node* a, Node* b) {
        if (a) a->next = b;
        if (b) b->prev = a;
    }

    void ensureBucketSize(int size) {
        if (freqBuckets.size() <= size)
            freqBuckets.resize(size + 1);
    }

    void addPair(Node* node) {
        if (!node || !node->next) return;
        Pair p = {node->symbol, node->next->symbol};

        // Prevent re-adding replaced pairs
        if (replacedPairs.count(p)) return;

        pairPositions[p].push_back(node);
        int freq = ++pairFreq[p];
        ensureBucketSize(freq);
        freqBuckets[freq].push_back(p);
        maxFreq = max(maxFreq, freq);
    }

    void removePair(Node* node) {
        if (!node || !node->next) return;
        Pair p = {node->symbol, node->next->symbol};
        auto& listRef = pairPositions[p];
        listRef.remove(node);

        int freq = pairFreq[p];
        freqBuckets[freq].remove(p);
        if (--freq == 0) {
            pairFreq.erase(p);
            pairPositions.erase(p);
        } else {
            freqBuckets[freq].push_back(p);
        }

        while (maxFreq > 0 && freqBuckets[maxFreq].empty()) maxFreq--;
    }

    void replacePair(const Pair& p, char newSymbol) {
        replacedPairs.insert(p); // Record that this pair is now replaced

        auto it = pairPositions.find(p);
        if (it == pairPositions.end()) return;

        // Make a copy of occurrences to avoid iterator invalidation
        list<Node*> occurrences = it->second;

        for (Node* left : occurrences) {
            Node* right = left ? left->next : nullptr;
            if (!left || !right) continue;

            // Sanity check — skip if no longer valid
            if (left->symbol != p.first || right->symbol != p.second) continue;
            if (left->next != right || right->prev != left) continue;

            // Remove surrounding pairs
            if (left->prev) removePair(left->prev);
            removePair(left);
            removePair(right);

            // Create new symbol node
            Node* newNode = new Node(newSymbol);
            Node* before = left->prev;
            Node* after = right->next;

            link(before, newNode);
            link(newNode, after);

            if (left == head) head = newNode;
            if (right == tail) tail = newNode;

            delete left;
            delete right;

            // Add new adjacent pairs
            if (newNode->prev) addPair(newNode->prev);
            if (newNode->next) addPair(newNode);
        }

        // Clean up metadata
        pairPositions.erase(p);
        pairFreq.erase(p);
        freqBuckets[maxFreq].remove(p);
        while (maxFreq > 0 && freqBuckets[maxFreq].empty()) maxFreq--;
    }

public:
    RepairCompressor(const string& input) {
        Node* prev = nullptr;
        for (char c : input) {
            Node* n = new Node(c);
            if (!head) head = n;
            link(prev, n);
            prev = n;
        }
        tail = prev;

        for (Node* curr = head; curr && curr->next; curr = curr->next) {
            addPair(curr);
        }
    }

    void compress() {
        while (maxFreq > 1) {
            while (maxFreq > 0 && freqBuckets[maxFreq].empty()) maxFreq--;
            if (maxFreq <= 1) break;

            Pair mostFreqPair = freqBuckets[maxFreq].front();
            cout << "Replacing pair: (" << mostFreqPair.first << ", " << mostFreqPair.second
                 << ") with symbol: " << (int)nextSymbol << endl;

            replacePair(mostFreqPair, nextSymbol++);
        }
    }

    void printCompressedSequence() {
        cout << "Final compressed sequence (char values): ";
        for (Node* curr = head; curr; curr = curr->next) {
            if (curr->symbol >= 32 && curr->symbol < 127)
                cout << curr->symbol << " ";
            else
                cout << "(" << (int)curr->symbol << ") ";
        }
        cout << endl;
    }

    ~RepairCompressor() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }
};

int main() {
    string input = "abcdabcdabcdabcababab";
    RepairCompressor compressor(input);
    compressor.compress();
    compressor.printCompressedSequence();
    return 0;
}
