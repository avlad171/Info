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
    string tempname = "";
    for(int tries = 0; tries < 10; ++tries)
    {
        //generate random name
        uint64_t rn = (rand() << 16) | rand();
        tempname = "temp" + to_hex_string(rn);
        cout<<"Generated temp file name: "<<tempname<<"\n";
        //try to open file
        temp.open(tempname, ios::in | ios::out | ios::trunc | ios::binary);

        if(!temp.fail())    //successful open
            break;

        tempname.clear();
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
        char * inbuf = new char [1024];
        char * outbuf = new char [4096];    //max huffman code size is 31 bits so maximum inflation is 4 times

        while(true)
        {
            raw.read(inbuf, 1024);
            int readBytes = raw.gcount();

            int writeBytes = 0;

            if(raw.eof())
            {
                LZW.compressFinal(inbuf, outbuf, readBytes, writeBytes);
                temp.write(outbuf, writeBytes);

                break;
            }

            else
            {
                LZW.compress(inbuf, outbuf, readBytes, writeBytes);
                temp.write(outbuf, writeBytes);
            }
        }

        uint64_t inputsize = LZW.getInputSize();
        uint64_t tempsize = LZW.getOutputSize();

        LZW.printInfo();
        LZW.reset();

        //start huffman compression

        //reset the input file
        temp.clear();
        temp.seekg(0);

        //parse the input file and update freqs
        while(true)
        {
            temp.read(inbuf, 1024);
            int readBytes = temp.gcount();

            HFC.updateFrequency(inbuf, readBytes);

            if(temp.eof())
                break;
        }

        //build tree
        HFC.buildTree();

        //output tree in the output file
        char * header = new char [266];
        header[0] = 'H';
        header[1] = 'C';
        HFC.serialize(header + 10);
        out.write(header, 266);
        delete [] header;

        //reset the input file again
        temp.clear();
        temp.seekg(0);

        //do actual compression
        while(true)
        {
            int writeBytes = 0;

            //read packet
            temp.read(inbuf, 1024);
            int readBytes = temp.gcount();

            if(temp.eof())
            {
                HFC.compressFinal(inbuf, outbuf, readBytes, writeBytes);
                out.write(outbuf, writeBytes);
                break;
            }

            else
            {
                HFC.compress(inbuf, outbuf, readBytes, writeBytes);
                out.write(outbuf, writeBytes);
            }
        }

        //write uncompressed size in header
        out.seekp(2);
        out.write((const char*)&tempsize, 8);

        //final info
        uint64_t outputsize = HFC.getOutputSize();
        cout<<"Compressed file size is "<<outputsize<<" ("<<((double)outputsize / (double)inputsize) * 100.0d<<"% of input size)\n";

        //free memory
        delete [] inbuf;
        delete [] outbuf;

        //close files
        raw.close();
        temp.close();
        out.close();

        //delete temp file
        remove(tempname.c_str());
    }

    else if(!strcmp(argv[1], "-d"))  //decompress
    {
        char * inbuf = new char [1024];

        //read header
        char * header = new char [266];
        raw.read(header, 266);

        if(raw.eof() || raw.gcount() < 266)
        {
            cout<<"Error: unable to parse huffman header!\n";
            return -1;
        }

        if(header[0] != 'H' || header[1] != 'C')
        {
            cout<<"Error: header file corrupted!\n";
            return -1;
        }

        //read output file size
        uint64_t tempsize = 0;

        memcpy(&tempsize, header + 2, 8);
        cout<<"Temp file size is "<<tempsize<<"\n";

        //build tree from header
        HFC.deserialize(header + 10);
        delete [] header;

        //read and decompress data
        while(true)
        {
            raw.read(inbuf, 1024);
            int readBytes = raw.gcount();

            string outbuf = "";

            if(raw.eof())
            {
                HFC.decompressFinal(inbuf, readBytes, outbuf);
                temp.write(outbuf.data(), min(outbuf.size(), tempsize));
                tempsize -= min(outbuf.size(), tempsize);

                outbuf.clear();
                break;
            }

            else
            {
                HFC.decompress(inbuf, readBytes, outbuf);
                temp.write(outbuf.data(), min(outbuf.size(), tempsize));
                tempsize -= min(outbuf.size(), tempsize);

                outbuf.clear();
            }
        }

        //reset temp file
        temp.clear();
        temp.seekg(0);

        int readBytes = 0;

        while(true)
        {
            temp.read(inbuf + readBytes, 1024 - readBytes);
            readBytes += temp.gcount();

            cout<<"Read "<<readBytes<<" bytes\n";
            bstring outbuf;
            int processed = LZW.decompress(inbuf, readBytes, outbuf);

            memcpy(inbuf, inbuf + processed, 1024 - processed);
            readBytes = 1024 - processed;

            out.write((const char*)outbuf.data(), outbuf.size());
            outbuf.clear();

            if(temp.eof())
                break;
        }

        LZW.printInfo();

        delete [] inbuf;

        //close files
        raw.close();
        temp.close();
        out.close();

        //delete temp file
        remove(tempname.c_str());
    }

    else if(!strcmp(argv[1], "-h")) //huffman debug compress
    {
        char * inbuf = new char [1024];
        char * outbuf = new char [4096];
        uint64_t filesize = 0;

        //parse the input file and update freqs
        while(true)
        {
            raw.read(inbuf, 1024);
            int readBytes = raw.gcount();

            HFC.updateFrequency(inbuf, readBytes);

            if(raw.eof())
                break;
        }

        //build tree
        HFC.buildTree();

        //output tree in the file
        char * header = new char [266];
        header[0] = 'H';
        header[1] = 'C';
        HFC.serialize(header + 10);
        temp.write(header, 266);
        delete [] header;

        //reset the input file
        raw.clear();
        raw.seekg(0);

        //do actual compression
        while(true)
        {
            int writeBytes = 0;

            //read packet
            raw.read(inbuf, 1024);
            int readBytes = raw.gcount();

            //increment file size
            filesize += readBytes;

            if(raw.eof())
            {
                HFC.compressFinal(inbuf, outbuf, readBytes, writeBytes);
                temp.write(outbuf, writeBytes);

                break;
            }

            else
            {
                HFC.compress(inbuf, outbuf, readBytes, writeBytes);
                temp.write(outbuf, writeBytes);
            }
        }

        //write uncompressed size in header
        temp.seekp(2);
        temp.write((const char*)&filesize, 8);

        temp.close();
        delete [] inbuf;
        delete [] outbuf;
    }

    else if (!strcmp(argv[1], "-hd"))   //huffman debug decompress
    {
        char * inbuf = new char [1024];

        //read header
        char * header = new char [266];
        raw.read(header, 266);

        if(raw.eof() || raw.gcount() < 266)
        {
            cout<<"Error: unable to parse huffman header!\n";
            return -1;
        }

        if(header[0] != 'H' || header[1] != 'C')
        {
            cout<<"Error: header file corrupted!\n";
            return -1;
        }

        //read output file size
        uint64_t outputFileSize;
        memcpy(&outputFileSize, header + 2, 8);

        //build tree from header
        HFC.deserialize(header + 10);
        delete [] header;

        //read and decompress data
        while(true)
        {
            raw.read(inbuf, 1024);

            int readBytes = raw.gcount();

            string plm;

            HFC.decompress(inbuf, readBytes, plm);

            cout.write(plm.data(), min(plm.size(), outputFileSize));
            plm.clear();
            if(raw.eof())
                break;
        }

        delete [] inbuf;
        cout<<"\n\n";

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
