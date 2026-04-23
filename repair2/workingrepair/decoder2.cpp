#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <sdsl/bit_vectors.hpp>
#include <math.h>
#include <queue>


using namespace std;

struct decoded
{
    sdsl::bit_vector bit;
    uint32_t cfgSize;   
};



decoded decodeBitfile(string path)
{
    ifstream enc(path, ios::binary);
    
    uint32_t bitCount = 0;
    uint32_t cfgSize = 0;
    
    enc.read(reinterpret_cast<char*>(&bitCount), sizeof(bitCount));
    enc.read(reinterpret_cast<char*>(&cfgSize), sizeof(cfgSize));

    vector<char> buffer((istreambuf_iterator<char>(enc)), {});
    
    sdsl::bit_vector bitString(bitCount);
    
    size_t bitIndex = 0;
    for (size_t i = 0; i < buffer.size(); ++i)
    {
        
        unsigned char byte = static_cast<unsigned char>(buffer[i]);
        
        for (int b = 7; b >= 0; --b)
        {
            if (bitIndex >= bitCount) break;
            bitString[bitIndex++] = (byte >> b) & 1;
        }
    }
    decoded bs;
    bs.cfgSize = cfgSize;
    bs.bit = bitString;
    return bs;
}


uint decodeSymbol(const sdsl::bit_vector& bits, size_t& i, vector<unsigned>& rules, uint& ruleCount)
{
    if (i >= bits.size()) 
        return -1;

    if (bits[i] == 1)
    {
        i++;

        uint left = decodeSymbol(bits, i, rules, ruleCount);
        uint right = decodeSymbol(bits, i, rules, ruleCount);

        // Ensure the rules vector is large enough
        size_t requiredSize = (ruleCount + 1) * 2;  // Next rule will need 2 slots
        if (rules.size() < requiredSize) {
            rules.resize(requiredSize);
        }
        
        // Store rule at position ruleCount * 2
        rules[ruleCount * 2] = left;
        rules[ruleCount * 2 + 1] = right;
        
        uint newSymbol = 256 + ruleCount;
        ruleCount++;
        
        return newSymbol;
    }
    else
    {
        i++;
        int w = static_cast<int>(ceil(log2(256.0 + ruleCount)));
        
        if (i + w > bits.size()) 
            return -1;

        unsigned val = 0;
        
        for (int j = 0; j < w; j++)
        {
            val = (val << 1) | bits[i];
            i++;
        }
        
        return val;
    }
}

void expandSymbol(uint sym, const vector<uint>& rules, vector<uint>& output)
{
    if (sym < 256) {
        // Terminal symbol — just output it
        output.push_back(sym);
    } else {
        // Non-terminal: look up its rule
        size_t idx = (sym - 256) * 2;
        if (idx + 1 >= rules.size()) {
            // Safety check: invalid rule
            return;
        }
        uint left = rules[idx];
        uint right = rules[idx + 1];
        
        // Recursively expand left, then right
        expandSymbol(left, rules, output);
        expandSymbol(right, rules, output);
    }
}

vector<uint> expandCFG(const vector<uint>& cfg, const vector<uint>& rules)
{
    vector<uint> result;
    for (uint sym : cfg) {
        expandSymbol(sym, rules, result);
    }
    return result;
}



int main(void)
{
    decoded bit = decodeBitfile("saida.rpr");

    vector<unsigned> rules;
    vector<unsigned> cfg;
    uint ruleCount = 0;
    size_t i = 0;

    for (uint32_t k = 0; k < bit.cfgSize; k++)
    {
        uint symbol = decodeSymbol(bit.bit, i, rules, ruleCount);
        cfg.push_back(symbol);
    }

    // Now expand using the original expandSymbol function (it should work now)
    vector<uint> originalSymbols = expandCFG(cfg, rules);

    cout << "Rules vector size: " << rules.size() << endl;
    cout << "Rule count: " << ruleCount << endl;
    cout << "CFG size: " << cfg.size() << endl;
    
    cout << "Original symbols: ";
    for (uint s : originalSymbols) {
        cout << s << " ";
    }
    cout << "\n";

    cout << "As string: \"";
    for (uint s : originalSymbols) {
        if (s >= 32 && s <= 126) cout << static_cast<char>(s);
        else cout << '?';
    }
    cout << "\"\n";
    return 0;
}