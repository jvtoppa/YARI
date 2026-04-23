
#include <string>
#include <fstream>
#include "encodeCFG.hpp"
#include <math.h>
#include <algorithm>
typedef unsigned int uint;
typedef basic_string<uint> uintstring;
using namespace std;

namespace CFG
{

void writeToDisk(const string& bitString, const string& outputPath)
{

    u_int32_t bits = bitString.size();

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
        /*
        bitString.push_back('o');
        bitString.push_back(' ');
        */
        encodeCFG_rec(ruleHistory[leftRule], ruleHistory, bitString, rulesEncoded, seen);
        encodeCFG_rec(ruleHistory[rightRule], ruleHistory, bitString, rulesEncoded, seen);
        rulesEncoded++;
        //sy = rulesEncoded;
    }
    else
    {   
        bitString.push_back('0');
        /*
        bitString.push_back('z');
        bitString.push_back(' ');
        */

        writeuint(sy, bitString, rulesEncoded);
    }
}

void encodeCFG(uint& sy, uintstring& ruleHistory, string& bitString, uint& rulesEncoded, unordered_set<uint>& seen)
{
    encodeCFG_rec(sy, ruleHistory, bitString, rulesEncoded, seen);

    bitString.push_back('1');
    /*
    bitString.push_back('w');
    bitString.push_back(' ');
    */
}


}