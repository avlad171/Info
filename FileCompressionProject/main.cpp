#include <iostream>
#include <fstream>

#include "lzw.h"

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

    ofstream compStage1 ("lzw.dat");

    lzw LZW(12);

    char * inbuf = new char [1024];
    char * outbuf = new char [1536];

    while(true)
    {
        raw.read(inbuf, 1024);
        int readBytes = raw.gcount();

        int writeBytes = 0;

        if(raw.eof())
        {
            LZW.compressFinal(inbuf, outbuf, readBytes, writeBytes);
            cout<<"The output is "<<writeBytes<<" bytes in size\n";
            cout<<"Binary output: ";
            cout.write(outbuf, writeBytes);
            compStage1.write(outbuf, writeBytes);

            cout<<endl;
            break;
        }

        else
        {
            cout<<"Not eof!\n";
            LZW.compress(inbuf, outbuf, readBytes, writeBytes);
        }
    }


    delete [] inbuf;
    delete [] outbuf;
    return 0;
}
