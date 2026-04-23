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


uint decodeSymbol(const sdsl::bit_vector& bits, size_t& i, uint& characFull, vector<unsigned>& rules)
{
    if (i >= bits.size()) 
        return -1;

    if (bits[i] == 1)
    {
        i++;

        uint left = decodeSymbol(bits, i, characFull, rules);
        uint right = decodeSymbol(bits, i, characFull, rules);

        rules.push_back(left);
        rules.push_back(right);
        uint currRule = characFull;
        characFull++;

        return currRule;
    }
    else
    {
        i++;
        int w = static_cast<int>(ceil(log2(static_cast<double>(characFull))));
        
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





int main(void)
{
    decoded bit = decodeBitfile("saida.rpr");

    vector<unsigned> rules;
    vector<unsigned> cfg;
    uint characFull = 256;
    size_t i = 0;


    for (uint32_t k = 0; k < bit.cfgSize; k++)
    {
        cfg.push_back(decodeSymbol(bit.bit, i, characFull, rules));
        if (i < bit.bit.size() && bit.bit[i] == 1) {
            i++;
        }
    }
    cout << "Rules: ";
    for(unsigned n : rules)
    {

        cout << n << " ";

    }
    cout << "\nCFG: ";
    for(unsigned n : cfg)
    {

        cout << n << " ";

    }
    return 0;
}