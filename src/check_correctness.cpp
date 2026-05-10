#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <file1> <file2>" << endl;
        return 1;
    }

    ifstream file1(argv[1], ios::binary);
    ifstream file2(argv[2], ios::binary);
    
    if (!file1 || !file2) {
        cerr << "Error: File opening failed." << endl;
        return 1;
    }

    char ch1, ch2;
    size_t pos = 0;
    bool identical = true;

    while (file1.get(ch1) && file2.get(ch2)) {
        if (ch1 != ch2) {
            cout << "Difference found at byte " << pos << ". Capturing remainder..." << endl;
            identical = false;
            
            // Create a file to save the divergent data
            ofstream diffFile("diverged_content.bin", ios::binary);
            
            // 1. Save the very first byte that was different
            diffFile.put(ch1);
            
            // 2. Efficiently dump the rest of file1 into the diffFile
            // .rdbuf() is the fastest way to pipe one stream into another
            diffFile << file1.rdbuf(); 
            
            diffFile.close();
            break; 
        }
        pos++;
    }

    if (identical) {
        // Special case: file1 is longer than file2
        if (file1.get(ch1)) {
            cout << "File 1 is longer than File 2. Capturing extra bytes..." << endl;
            ofstream diffFile("extra_bytes.bin", ios::binary);
            diffFile.put(ch1);
            diffFile << file1.rdbuf();
        } else {
            cout << "Files are identical." << endl;
        }
    }

    return 0;
}