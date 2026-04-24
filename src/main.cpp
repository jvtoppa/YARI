#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include <chrono>
#include "Repair.h"

using namespace std;

int main()
{
    //string input = "singing do wah diddy diddy dum diddy do";
    //string input = "banaananaanana";
    string input = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
    auto start = chrono::high_resolution_clock::now();

    Repair repair(input);
    repair.run(false);
    
    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double, nano> time2 = end2 - start;
    cout << "Time to compress: " << time2.count() / input.size() << "ns. / char\n";
    repair.output();
    return 0;
}