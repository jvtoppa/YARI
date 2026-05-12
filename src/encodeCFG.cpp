#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <cstdint>

typedef size_t GrammarUnit; 
typedef std::vector<GrammarUnit> UnitVector; 

using namespace std;

namespace CFG
{
    void writeToDisk(const string& bitString, const string& outputPath)
    {
        uint32_t bits = static_cast<uint32_t>(bitString.size());
        ofstream out(outputPath, ios::binary);
        unsigned char buffer = 0;
        int bitCount = 0;

        out.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
        for (char bit : bitString) 
        {
            buffer <<= 1;
            if (bit == '1') buffer |= 1;
            bitCount++;

            if (bitCount == 8) {
                out.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }

        if (bitCount > 0)
        {
            buffer <<= (8 - bitCount);
            out.put(buffer);
        }
        out.close();
    }

    void writeuint(GrammarUnit s, string& bitString, const GrammarUnit& rulesEncoded)
    {
        // Calculate bits needed to represent rulesEncoded
        int bitsLen = (rulesEncoded <= 1) ? 1 : static_cast<int>(floor(log2(rulesEncoded))) + 1;
        
        for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
        {
            bitString.push_back(((s >> bitPosition) & 1) ? '1' : '0');
        }
    }

    void encodeCFG_rec(GrammarUnit& sy, const UnitVector& ruleHistory, string& bitString, GrammarUnit& rulesEncoded, unordered_set<GrammarUnit>& seen)
    {
        if(sy >= 256 && seen.find(sy) == seen.end())
        {
            seen.insert(sy);
            GrammarUnit leftRule = 2 * (sy - 256);
            GrammarUnit rightRule = 2 * (sy - 256) + 1;

            if(rightRule >= ruleHistory.size())
            {
                cerr << "Error: Invalid rule index for symbol " << sy << endl;
                return;
            }
            
            bitString.push_back('1');
            
            GrammarUnit left = ruleHistory[leftRule];
            GrammarUnit right = ruleHistory[rightRule];
            
            encodeCFG_rec(left, ruleHistory, bitString, rulesEncoded, seen);
            encodeCFG_rec(right, ruleHistory, bitString, rulesEncoded, seen);
            rulesEncoded++;
        }
        else
        {   
            bitString.push_back('0');
            writeuint(sy, bitString, rulesEncoded);
        }
    }

    UnitVector read_sequence(bool verbose = false)
    {
        ifstream file("output/sequence.rp", ios::binary);
        size_t val;
        UnitVector read;

        while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
        {
            read.push_back(val);
            if(verbose) {
                if(val <= 255) cout << static_cast<char>(val);
                else cout << "[" << val << "]";
            }
        }
        return read;
    }

    UnitVector read_rulehistory()
    {
        ifstream file("output/rulehistory.rp", ios::binary);
        size_t val;
        UnitVector read;
        while (file.read(reinterpret_cast<char*>(&val), sizeof(size_t)))
        {
            read.push_back(val);
        }
        return read;
    }
}

int main()
{
    UnitVector seq = CFG::read_sequence();
    UnitVector rulehistory = CFG::read_rulehistory();

    if (seq.empty()) {
        cerr << "Error: Sequence file is empty or missing." << endl;
        return 1;
    }

    string bitString = "";
    GrammarUnit rulesEncoded = 256; 
    unordered_set<GrammarUnit> seen;

    for (GrammarUnit& sy : seq)
    {
        CFG::encodeCFG_rec(sy, rulehistory, bitString, rulesEncoded, seen);
    }
   
    cout << "Processed " << seq.size() << " sequence symbols." << endl;
    CFG::writeToDisk(bitString, "output/compressed_cfg.bin");

    return 0;
}