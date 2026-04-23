#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

using Pair = pair<char, char>;

struct PairHash {
    size_t operator()(const Pair& p) const {
        return hash<char>()(p.first) ^ (hash<char>()(p.second) << 1);
    }
};

// Count frequencies of adjacent character pairs
unordered_map<Pair, int, PairHash> countPairs(const vector<char>& seq) {
    unordered_map<Pair, int, PairHash> freqMap;
    for (size_t i = 0; i + 1 < seq.size(); ++i) {
        Pair p = {seq[i], seq[i + 1]};
        freqMap[p]++;
    }
    return freqMap;
}

// Replace all occurrences of a pair with a new symbol
void replacePair(vector<char>& seq, const Pair& p, char newChar) {
    vector<char> newSeq;
    size_t i = 0;
    while (i < seq.size()) {
        if (i + 1 < seq.size() && seq[i] == p.first && seq[i + 1] == p.second) {
            newSeq.push_back(newChar);
            i += 2;
        } else {
            newSeq.push_back(seq[i]);
            i++;
        }
    }
    seq = move(newSeq);

    // Optional: print intermediate sequence
    for (char c : seq) {
        if (c >= 32 && c < 127)
            cout << c << " ";
        else
            cout << "(" << (int)c << ") ";
    }
    cout << endl;
}

int main() {
    string input = "abcdefghhhh";
    vector<char> seq(input.begin(), input.end());

    char nextSymbol = 127; // Start replacing with extended ASCII or control characters

    while (true) {
        auto freqMap = countPairs(seq); // O(n)

        // Find the most frequent pair (with frequency > 1)
        int maxFreq = 1;
        Pair maxPair;
        for (const auto& entry : freqMap) {
            if (entry.second > maxFreq) {
                maxFreq = entry.second;
                maxPair = entry.first;
            }
        }

        if (maxFreq == 1) {
            break; // No pair appears more than once
        }

        cout << "Replacing pair: (" << maxPair.first << ", " << maxPair.second
             << ") with symbol: " << (int)nextSymbol << endl;

        replacePair(seq, maxPair, nextSymbol);
        nextSymbol++;
    }

    // Output final compressed sequence
    cout << "Final compressed sequence (char values): ";
    for (char c : seq) {
        if (c >= 32 && c < 127)
            cout << c << " ";
        else
            cout << "(" << (int)c << ") ";
    }
    cout << endl;

    return 0;
}
