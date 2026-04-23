
#include <string>
#include <fstream>
#include "encodeCFG.hpp"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <cstdint>

using namespace std;
typedef unsigned int uint;
typedef basic_string<uint> uintstring;

void writeuint(uint s, string& bitString, const uint& rulesEncoded)
{

    int bitsLen = static_cast<int>(ceil(log2(rulesEncoded)));
    string byteStr = "";
    
    for (int bitPosition = bitsLen - 1; bitPosition >= 0; --bitPosition)
    {
        unsigned char shiftedValue = s >> bitPosition;

        unsigned char singleBit = shiftedValue & 1;

        if (singleBit == 1)
        {
            bitString.push_back('1');
        }
        else
        {
            bitString.push_back('0');
        }
    }
    
    //bitString.push_back(' ');
    return;
}

void encodeCFG_rec(uint& sy, uintstring& ruleHistory, string& bitString, uint& rulesEncoded, unordered_set<uint>& seen)
{
    if(sy >= 256 && seen.find(sy) == seen.end())
    {
        seen.insert(sy);
        uint leftRule = 2 * (sy - 256);
        uint rightRule = 2 * (sy - 256) + 1;

        if(rightRule >= ruleHistory.size())
        {
            cerr << "Error: Invalid rule index for symbol " << sy << endl;
            return;
        }
        
        bitString.push_back('1');
        
      //  bitString.push_back('o');
    //    bitString.push_back(' ');
        
        encodeCFG_rec(ruleHistory[leftRule], ruleHistory, bitString, rulesEncoded, seen);
        encodeCFG_rec(ruleHistory[rightRule], ruleHistory, bitString, rulesEncoded, seen);
        rulesEncoded++;
        //sy = rulesEncoded;
    }
    else
    {   
        bitString.push_back('0');
        
    //    bitString.push_back(' ');
        

        writeuint(sy, bitString, rulesEncoded);
    }
}

void encodeCFG(uint& sy, uintstring& ruleHistory, string& bitString, uint& rulesEncoded, unordered_set<uint>& seen)
{
    encodeCFG_rec(sy, ruleHistory, bitString, rulesEncoded, seen);

    bitString.push_back('1');
    
  //  bitString.push_back(' ');
    
}

void writeToDisk(string bitString, uint32_t cfgSize, string outputPath)
{

u_int32_t bits = bitString.size();

ofstream out(outputPath, ios::binary);
unsigned char buffer = 0;
int bitCount = 0;
out.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
out.write(reinterpret_cast<const char*>(&cfgSize), sizeof(cfgSize));
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
if (bitCount > 0) {
    buffer <<= (8 - bitCount); 
    out.put(buffer);
}
}

int main(void)
{
    /*
    abcdeabcde
    AcdeAcde
    BdeBde
    CeCe
    DD

    -> stack:

    (e(d((ab)c)))259 mind = blown
    */
    uintstring CFG = {258,258};
    uintstring rules = {99,100,98,256,97,257};
    string bitStr = "";
    unordered_set<uint> seen;
    uint rulesEncoded = 256;
    /*
    auto append_uint32 = [&](uint32_t value, string& bitStr) {
        for (int i = 31; i >= 0; --i) {
            bitStr.push_back((value >> i) & 1 ? '1' : '0');
        }
    };
    
    append_uint32(rulesEncoded, bitStr);
    */
    cout << "[CONSOLE] Encoding CFG...\n";
    for (uint i : CFG)
    {
        encodeCFG(i, rules, bitStr, rulesEncoded, seen);

    }
    cout << "[CONSOLE] Done.\n";
    cout << bitStr << "\n";
    cout << "[CONSOLE] Writing to disk...\n";
    writeToDisk(bitStr, static_cast<uint32_t>(CFG.size()), "saida.rpr");
    cout << "[CONSOLE] Done.\n";
    cout << "[CONSOLE] Ending program.\n";
    

    return 0;
}