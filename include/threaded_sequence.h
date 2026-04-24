#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "pair.h"
using namespace std;

struct SEQ
{
    st code;
    st prev;
    st next;
};

class TSEQ
{
private:
    vector<SEQ> seq;
public:
    TSEQ(const string& s);
    ~TSEQ(){};
    void printConnections() const;
    st next(st pos);
    st prev(st pos);
    SEQ& operator[](st pos)
    {
        return seq[pos];
    }

    const SEQ& operator[](st pos) const
    {
        return seq[pos];
    }

    size_t size() const
    {
        return seq.size();
    }

    bool empty() const
    {
        return seq.empty();
    }

    TSEQ& operator=(vector<SEQ>&& new_vec) {
        this->seq = std::move(new_vec);
        return *this;
    }

};

inline TSEQ::TSEQ(const string& s)
{
    st string_size = s.size();
    seq.resize(string_size);
    for (st i = 0; i < string_size; i++)
    {
        seq[i].code = s[i];
    }
}


inline void TSEQ::printConnections() const
{
    for (size_t i = 0; i < seq.size(); i++)
    {
        auto printIndex = [](st idx) -> string
        {
            return (idx == N) ? "NULL" : to_string(idx);
        };

        cout << "SEQ " << i << " (code: " << seq[i].code << ")";

        if (seq[i].code == N)
        {
            cout << " -> Thread: " << printIndex(seq[i].next);
        }
        else
        {
            cout << " -> Next: " << printIndex(seq[i].next);
        }
        if (seq[i].code == N)
        {
            cout << ", Prev. Thread: " << printIndex(seq[i].prev) << "\n";
        }
        else
        {
            cout << ", Prev: " << printIndex(seq[i].prev) << "\n";
        }
    }
}

inline st TSEQ::next(st pos) {
    if (pos >= seq.size()) return N;
    st current = pos + 1;
    while (current < seq.size() && seq[current].code == N)
    {
        if (seq[current].next == N) return N;
        current = seq[current].next;
    }

    if (pos + 1 < seq.size() && seq[pos + 1].code == N) {
        seq[pos + 1].next = current;
    }
    return current;
}

inline st TSEQ::prev(st pos)
{
    if (pos == 0) return N;
    
    st current = pos - 1;
    
    while (current < seq.size() && seq[current].code == N)
    {
        if (seq[current].prev == N) return N;
    
        current = seq[current].prev;
    }
    
    return current;
}

#endif // SEQUENCE_H