#include <iostream>
#include <fstream>
#include <time.h>
#include <string>

#include "lzw.h"
#include "huffman.h"

using namespace std;

void printUsage()
{
    cout<<"Usage: LZW <mode> <inputfile> <outputfile>\n";
    cout<<"mode:\n";
    cout<<"\t\t -c to compress\n";
    cout<<"\t\t -d to decompress\n";
}

string to_hex_string(uint64_t x);

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        printUsage();
        return 0;
    }

    srand(time(0));

    //open file and check for errors
    ifstream raw (argv[2], ios::in | ios::binary);
    if(raw.fail())
    {
        cout<<"Unable to open input file!\n";
        return -1;
    }


    //create temp file and check for errors
    fstream temp;
    for(int tries = 0; tries < 10; ++tries)
    {
        //generate random name
        uint64_t rn = (rand() << 16) | rand();
        string tempname = "temp" + to_hex_string(rn);
        cout<<"Generated temp file name: "<<tempname<<"\n";
        //try to open file
        temp.open(tempname, ios::in | ios::out | ios::trunc | ios::binary);

        if(!temp.fail())    //successful open
            break;

        if(tries == 9)
        {
            cout<<"Unable to create temp file\n";
            return 0;
        }
    }

    //create output file
    ofstream out(argv[3], ios::out | ios::binary);
    if(out.fail())
    {
        cout<<"Unable to open input file!\n";
        return -1;
    }

    //create instance of lzw compressor
    lzw LZW(12);

    //create instance of huffman compressor
    huffmanCompressor HFC;

    if(!strcmp(argv[1], "-c"))  //compress
    {
        //ofstream compStage1 ("lzw.dat", ios::out | ios::binary);

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
                HFC.updateFrequency(inbuf, readBytes);
                //cout.write(outbuf, writeBytes);
                temp.write(outbuf, writeBytes);

                cout<<endl;
                break;
            }

            else
            {
                LZW.compress(inbuf, outbuf, readBytes, writeBytes);
                HFC.updateFrequency(inbuf, readBytes);
                //cout.write(outbuf, writeBytes);
                temp.write(outbuf, writeBytes);
            }
        }
        LZW.printInfo();
        LZW.reset();

        HFC.buildTree();

        delete [] inbuf;
        delete [] outbuf;

        temp.close();
    }

    else if(!strcmp(argv[1], "-d"))  //decompress
    {
        //ofstream decompStage1("decomp.txt", ios::out | ios::binary);

        char * inbuf = new char [1024];
        bstring outbuf;

        int readBytes = 0;


        while(true)
        {
            raw.read(inbuf + readBytes, 1024 - readBytes);
            readBytes += raw.gcount();

            int processed = LZW.decompress(inbuf, readBytes, outbuf);
            cout<<dec;
            //cout.write((const char*)outbuf.data(), outbuf.size());
            //cout<<"\n";

            memcpy(inbuf, inbuf + processed, 1024 - processed);
            readBytes = 1024 - processed;

            temp.write((const char*)outbuf.data(), outbuf.size());
            outbuf.clear();

            if(raw.eof())
                break;
        }

        LZW.printInfo();
        LZW.reset();

        delete [] inbuf;
        temp.close();
    }

    else
    {
        printUsage();
        return 0;
    }

    return 0;
}

string to_hex_string(uint64_t x)
{
    string ans = "0x";

    bool reachedNonzeroBit = false;

    for(int nibble_id = 15; nibble_id >= 0; --nibble_id)
    {
        uint64_t mask = 0xF;
        mask = mask << (4 * nibble_id);

        uint64_t extractedDigit = x & mask;
        extractedDigit = extractedDigit >> (4 * nibble_id);

        if(extractedDigit)
            reachedNonzeroBit = true;

        if(!reachedNonzeroBit)
            continue;

        if(extractedDigit < 10)
            ans += (char)(extractedDigit + 48);

        else
            ans += (char)(extractedDigit + 55);
    }

    if(!reachedNonzeroBit)
        ans += '0';

    return ans;
}
