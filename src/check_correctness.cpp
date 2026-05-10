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
            
            ofstream diffFile("diverged_content.bin", ios::binary);
            
            diffFile.put(ch1);
            
            diffFile << file1.rdbuf(); 
            
            diffFile.close();
            break; 
        }
        pos++;
    }

    if (identical)
    {
        cout << "Files are identical." << endl;
    }

    return 0;
}