#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>

using namespace std;
typedef unsigned int uint;
typedef basic_string<uint> uintstring;

void writeuint(uint s, string& bitString, const uint& rulesEncoded)
{
    // Handle edge case: rulesEncoded = 1 → log2(1)=0
    int bitsLen = (rulesEncoded <= 1) ? 1 : static_cast<int>(ceil(log2(static_cast<double>(rulesEncoded))));
    
    for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
    {
        unsigned char singleBit = (s >> bitPosition) & 1;
        bitString.push_back(singleBit ? '1' : '0');
    }
}

void encodeCFG_rec(
    uint sy,
    const uintstring& ruleHistory,
    string& bitString,
    uint& rulesEncoded,
    unordered_set<uint>& seen,
    unordered_map<uint, uint>& originalToDynamic)
{
    if (sy < 256) {
        // Terminal: write as-is
        bitString.push_back('0');
        writeuint(sy, bitString, rulesEncoded);
        return;
    }

    if (seen.find(sy) == seen.end())
    {
        // First time seeing this non-terminal
        seen.insert(sy);
        uint dynamicId = rulesEncoded;
        originalToDynamic[sy] = dynamicId;

        // Bounds check
        uint leftIdx = 2 * (sy - 256);
        uint rightIdx = leftIdx + 1;
        if (rightIdx >= ruleHistory.size()) {
            cerr << "Error: Invalid rule index for symbol " << sy << endl;
            return;
        }

        bitString.push_back('1');
        encodeCFG_rec(ruleHistory[leftIdx], ruleHistory, bitString, rulesEncoded, seen, originalToDynamic);
        encodeCFG_rec(ruleHistory[rightIdx], ruleHistory, bitString, rulesEncoded, seen, originalToDynamic);
        rulesEncoded++; // Assign dynamic ID after children
    }
    else
    {
        // Already seen: write dynamic ID
        bitString.push_back('0');
        uint dynamicId = originalToDynamic[sy];
        writeuint(dynamicId, bitString, rulesEncoded);
    }
}

void encodeCFG(
    uint sy,
    const uintstring& ruleHistory,
    string& bitString,
    uint& rulesEncoded,
    unordered_set<uint>& seen,
    unordered_map<uint, uint>& originalToDynamic)
{
    encodeCFG_rec(sy, ruleHistory, bitString, rulesEncoded, seen, originalToDynamic);
    bitString.push_back('1'); // end marker
}

void writeToDisk(string bitString, uint32_t cfgSize, string outputPath)
{
    uint32_t bits = static_cast<uint32_t>(bitString.size());
    ofstream out(outputPath, ios::binary);
    out.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
    out.write(reinterpret_cast<const char*>(&cfgSize), sizeof(cfgSize));

    unsigned char buffer = 0;
    int bitCount = 0;
    for (char bit : bitString) {
        buffer <<= 1;
        if (bit == '1') buffer |= 1;
        bitCount++;
        if (bitCount == 8) {
            out.put(buffer);
            buffer = 0;
            bitCount = 0;
        }
    }
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        out.put(buffer);
    }
}

int main(void)
{
    uintstring CFG = {115,263,263,256,111,32,119,97,104,261,260,117,109,261,111};
    uintstring rules = {32,100,121,256,100,257,100,258,105,259,256,260,110,103,105,262};
    
    string bitStr = "";
    unordered_set<uint> seen;
    unordered_map<uint, uint> originalToDynamic;
    uint rulesEncoded = 256;

    cout << "[CONSOLE] Encoding CFG...\n";
    for (uint sym : CFG) {
        encodeCFG(sym, rules, bitStr, rulesEncoded, seen, originalToDynamic);
    }
    cout << "[CONSOLE] Done.\n";
    cout << "[DEBUG] Bitstream length: " << bitStr.size() << " bits\n";
    cout << "[CONSOLE] Writing to disk...\n";
    writeToDisk(bitStr, static_cast<uint32_t>(CFG.size()), "saida.rpr");
    cout << "[CONSOLE] Done.\n";

    return 0;
}