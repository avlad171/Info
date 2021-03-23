#include <iostream>
#include <fstream>

#include "trie.h"

using namespace std;

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        cout<<"Usage: LZW <file to compress>\n";
        return 0;
    }

    //open file and check for errors
    ifstream raw (argv[1], ios::in | ios::binary);
    if(raw.fail())
    {
        cout<<"Unable to open file!\n";
        return -1;
    }

    //LZW init
    trie T;

    //step 1: insert every single character
    for (int i = 0; i < 256; ++i)
    {
        //cout<<"Inserting "<<i<<"\n";
        T.insert((const char*)&i, 1);
    }
    cout<<"[DEBUG] Single character inserion done!\n";

    //read first char from the file and initialize position in trie

    char P;
    raw.read(&P, 1);
    if(raw.eof())
    {
        cout<<"File is only 1 byte long! TBA\n";
        return 0;
    }
    cout<<"[DEBUG] Read "<<P<<"\n";
    int last_code = T.find_incremental(P);
    cout<<"First find incremental: "<<last_code;
    cout<<endl;

    //process the file
    while(true)
    {
        raw.read(&P, 1);

        if(raw.eof())
        {
            cout<<"Last code before EOF: "<<last_code<<"\n";
            break;
        }

        cout<<"[DEBUG] Read "<<P<<"\n";
        //if dictionary is at max size
        //TODO

        //else try to also append
        int code = T.find_incremental_and_add(P);
        if(code != -1)   //found the word, try with more chars
        {
            last_code = code;
        }
        else    //did not find the word, output its code
        {
            cout<<"Outputting code "<<last_code<<"\n";
            last_code = T.find_incremental(P);
        }
    }
    T.print();
    return 0;
}
