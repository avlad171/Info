#include "huffman.h"
#include <iostream>

using namespace std;

huffmanCompressor::huffmanCompressor()
{
    //create huffman table and links
    table = new tableEntry [256];
    for(int i = 0; i < 256; ++i)
        table[i].ch = (unsigned char)i;

    links = new int [256];

    //initialize sentinel node
    inf.sum = 0x7fffffff;

    //initialize internal data
    actual_char_number = 0;
    carry = 0;
    nr_carry_bits = 0;
    tree_built = 0;
    inputsize = 0;
    outputsize = 0;
    q1 = 0;
    q2 = 0;
}

int huffmanCompressor::updateFrequency(char* src, int sz)
{
    unsigned char * data = (unsigned char*)src;
    for(int i = 0; i < sz; ++i)
    {
        table[data[i]].frequency++;
    }

    return sz;
}

int huffmanCompressor::buildTree()
{
    //sort the table
    sort(table, table + 256);

    for(int i = 0; i < 256; ++i)
    {
        links[table[i].ch] = (unsigned char)i;

        temp.sum = table[i].frequency;
        if (temp.sum != 0)  //the character actually appears
        {
            temp.pos = i;
            Q1[actual_char_number] = temp;
            //cout<<"INSEREZ IN COADA "<<table[i].ch<<" CU FRECV "<<temp.sum<<"\n";
            ++actual_char_number;
        }
    }

    //set sentinel
    Q1[actual_char_number] = inf;

    //initialize second Q
    for(int i = 0; i <= actual_char_number; ++i)
        Q2[i] = inf;

    //actual tree building
    int cnt = 0;
    hNode* x [2];
    while(cnt < actual_char_number - 1)    //still have unprocessed nodes
    {
        x[0] = NULL;
        x[1] = NULL;

        for(int i = 0; i < 2; ++i)
        {
            if(Q1[q1].sum < Q2[q2].sum)
            {

                x[i] = &Q1[q1];
                //cout<<"IAU ELEMENTUL CU FRECVENTA "<<x[i]->sum<<"("<<q1<<") DIN COADA Q1\n";
                ++q1;
            }
            else
            {
                x[i] = &Q2[q2];
                //cout<<"IAU ELEMENTUL DE LEGATURA CU FRECVENTA "<<x[i]->sum<<"("<<q2<<") DIN COADA Q2\n";
                ++q2;
            }
        }
        temp.st = x[0];
        temp.dr = x[1];
        temp.sum = x[0]->sum + x[1]->sum;
        temp.pos = -1;

        //cout<<temp.st->sum<<" "<<temp.dr->sum<<" "<<temp.sum<<endl;

        Q2[cnt] = temp;
        ++cnt;
    }

    //generate codes
    DF(&Q2[q2], 0, 0);

    //debug - print
    for(int i = 0; i < 256; ++i)
        cout<<table[i].ch<<" "<<table[i].frequency<<" "<<table[i].code_len<<" "<<table[i].code<<"\n";

    //mark as initialized
    tree_built = 1;

    return 1;
}

void huffmanCompressor::DF(hNode* x, int64_t code, int16_t lvl)
{
    if(x->dr != nullptr && x->st != nullptr)
    {
        DF(x->st, code<<1, lvl + 1);
        DF(x->dr, code<<1 | 1, lvl + 1);
        return;
    }
    table[x->pos].code_len = lvl;
    table[x->pos].code = code;
    return;
}

huffmanCompressor::~huffmanCompressor()
{
    delete [] table;
    delete [] links;

    table = nullptr;
    links = nullptr;
}
