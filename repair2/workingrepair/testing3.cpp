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

// Helper function to read unsigned int from bits (matching encoder's writeuint)
uint readuint(const sdsl::bit_vector& bits, size_t& i, uint rulesEncoded)
{
    int bitsLen = (rulesEncoded <= 1) ? 1 : static_cast<int>(ceil(log2(static_cast<double>(rulesEncoded))));
    
    if (i + bitsLen > bits.size()) {
        return 0; // or handle error
    }
    
    uint val = 0;
    for (int j = 0; j < bitsLen; ++j)
    {
        val = (val << 1) | bits[i];
        i++;
    }
    return val;
}

uint decodeSymbol(const sdsl::bit_vector& bits, size_t& i, uint& characFull, vector<uint>& rules)
{
    if (i >= bits.size()) 
        return -1;

    if (bits[i] == 0) // Terminal or already seen non-terminal
    {
        i++;
        return readuint(bits, i, characFull);
    }
    else // New non-terminal (bit = 1)
    {
        i++; // Skip the '1' bit

        uint left = decodeSymbol(bits, i, characFull, rules);
        uint right = decodeSymbol(bits, i, characFull, rules);

        // Add the rule (left, right) to our rules vector
        rules.push_back(left);
        rules.push_back(right);
        
        // Return the ID for this new rule
        uint newRuleId = characFull;
        characFull++;
        return newRuleId;
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

    vector<uint> rules;
    vector<uint> cfg;
    uint characFull = 256;
    size_t i = 0;

    for (uint32_t k = 0; k < bit.cfgSize; k++)
    {
        uint symbol = decodeSymbol(bit.bit, i, characFull, rules);
        cfg.push_back(symbol);
        
        // Skip the end marker '1' that was added by encoder
        if (i < bit.bit.size() && bit.bit[i] == 1) {
            i++;
        }
    }

    // Expand CFG to original symbol sequence
    vector<uint> originalSymbols = expandCFG(cfg, rules);

    // Print results
    cout << "Rules: ";
    for (size_t j = 0; j < rules.size(); j += 2) {
        cout << "(" << rules[j] << "," << rules[j+1] << ") ";
    }
    cout << "\n";
    
    cout << "CFG: ";
    for (uint s : cfg) {
        cout << s << " ";
    }
    cout << "\n";
    
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