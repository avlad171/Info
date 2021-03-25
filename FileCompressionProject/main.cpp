#include <iostream>
#include <fstream>

#include "lzw.h"

using namespace std;

void printUsage()
{
    cout<<"Usage: LZW <mode> <inputfile> <outputfile>\n";
    cout<<"mode:\n";
    cout<<"\t\t -c to compress\n";
    cout<<"\t\t -d to decompress\n";
}

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printUsage();
        return 0;
    }

    //open file and check for errors
    ifstream raw (argv[2], ios::in | ios::binary);
    if(raw.fail())
    {
        cout<<"Unable to open input file!\n";
        return -1;
    }



    lzw LZW(12);

    if(!strcmp(argv[1], "-c"))  //compress
    {
        ofstream compStage1 ("lzw.dat", ios::out | ios::binary);

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
                cout.write(outbuf, writeBytes);
                compStage1.write(outbuf, writeBytes);

                cout<<endl;
                break;
            }

            else
            {
                LZW.compress(inbuf, outbuf, readBytes, writeBytes);
                cout.write(outbuf, writeBytes);
                compStage1.write(outbuf, writeBytes);
            }
        }
        LZW.printInfo();
        LZW.reset();

        delete [] inbuf;
        delete [] outbuf;

        compStage1.close();
    }

    else if(!strcmp(argv[1], "-d"))  //decompress
    {
        ofstream decompStage1("decomp.txt", ios::out | ios::binary);

        char * inbuf = new char [1024];
        char * outbuf = new char [2048];

        int readBytes = 0;

        while(true)
        {
            raw.read(inbuf + readBytes, 1024 - readBytes);
            readBytes += raw.gcount();

            int writeBytes = 0;

            int processed = LZW.decompress(inbuf, outbuf, readBytes, writeBytes);
            cout<<dec;
            cout.write(outbuf, writeBytes);
            cout<<"\n";

            memcpy(inbuf, inbuf + processed, 1024 - processed);
            readBytes = 1024 - processed;

            decompStage1.write(outbuf, writeBytes);

            if(raw.eof())
                break;
        }

        LZW.printInfo();
        LZW.reset();

        decompStage1.close();
    }

    else
    {
        printUsage();
        return 0;
    }

    return 0;
}
