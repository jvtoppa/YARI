#include <iostream>
#include <vector>
#include <climits>
#include <string>
#include <math.h>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include "repair.h"

using namespace std;

int main(int argc, char* argv[]) {
    
	std::string T;
	
	if (argc < 2)
	{
		std::ostringstream buffer;
		buffer << std::cin.rdbuf();
		T = buffer.str();
		cout << "Read " << T.size() << "charaters. \n";
	}
    else
	{
		unsigned long max_chars = std::stoul(argv[1]);
		
		
		T.resize(max_chars);
	
		std::cin.read(&T[0], max_chars);
		
		T.resize(std::cin.gcount());

	}

    auto start = chrono::high_resolution_clock::now();
    cout << "Starting...\n";
    Repair repair(T);
    repair.run(true);
    repair.output(true);
    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time2 = end2 - start;
    cout << "Time to compress: " << time2.count() << "ms.\n";

    //repair.output();
    return 0;
}

