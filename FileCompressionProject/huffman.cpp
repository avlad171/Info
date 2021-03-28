#include "huffman.h"
#include <iostream>
#include <string.h>

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

bool canonic_cmp(tableEntry &a, tableEntry &b)
{
    if(a.code_len == b.code_len)
        return a.ch < b.ch;
    return a.code_len < b.code_len;
}

bool alphabetic_cmp(tableEntry &a, tableEntry &b)
{
    return a.ch < b.ch;
}

int huffmanCompressor::buildTree()
{
    //sort the table
    sort(table, table + 256);

    for(int i = 0; i < 256; ++i)
    {
        links[table[i].ch] = (unsigned char)i;

        temp.sum = table[i].frequency;
        temp.ch = table[i].ch;

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
    root = &Q2[q2];
    DF(root, 0, 0);

    //initial debug print
    cout<<"Initial debug print\n";
    for(int i = 0; i < 256; ++i)
        cout<<table[i].ch<<" "<<table[i].frequency<<" "<<table[i].code_len<<" "<<table[i].code<<"\n";

    //truncate codes to max 31 bits
    int limitation = 0;
    for(int i = 0; i < 256; ++i)
        if(table[i].code_len > 31)
        {
            limitation = 1;
            cout<<"Warning: the optimal huffman encoding contains codes longer than 31 bits! In order to avoid this the compression will be a little sub-optimal!\n";
            break;
        }

    //make each code longer than 23 31 bits so the canonic generator can code each of them
    if(limitation)
        for(int i = 0; i < 256; ++i)
        if(table[i].code_len >= 24)
            table[i].code_len = 31;


    //make the table cannonic - https://en.wikipedia.org/wiki/Canonical_Huffman_code
    sort(table, table + 256, canonic_cmp);

    int new_code = 0;
    int old_len = 0;
    for(int i = 0; i < 256; ++i)
    {
        if(table[i].code_len == 0)
            continue;
        new_code = new_code << (table[i].code_len - old_len);
        old_len = table[i].code_len;

        table[i].code = new_code;
        new_code++;
    }

    sort(table, table + 256, alphabetic_cmp);

    //second debug print
    cout<<"Second debug print\n";
    for(int i = 0; i < 256; ++i)
        cout<<table[i].ch<<" "<<table[i].frequency<<" "<<table[i].code_len<<" "<<table[i].code<<"\n";

    //mark as initialized
    tree_built = 1;
    cout<<"Tree building done!\n";
    return 1;
}

void huffmanCompressor::DF(hNode* x, int64_t code, int16_t lvl)
{
    if(x->dr != nullptr && x->st != nullptr)
    {
        DF(x->st, code << 1, lvl + 1);
        DF(x->dr, code << 1 | 1, lvl + 1);
        return;
    }
    table[x->pos].code_len = lvl;
    table[x->pos].code = code;
    return;
}

void huffmanCompressor::insert_code(hNode* x, int64_t code, int16_t code_len, unsigned char ch)
{
    if(code_len == 0)
    {
        x->ch = ch;
        return;
    }

    bool bit = code & (1 << (code_len - 1));

    if(bit == 0)
    {
        if(x->st == nullptr)
        {
            x->st = &Q1[++actual_char_number];
            x->st->ch = -1;
        }

        insert_code(x->st, code, code_len - 1, ch);
    }

    else
    {
        if(x->dr == nullptr)
        {
            x->dr = &Q1[++actual_char_number];
            x->dr->ch = -1;
        }

        insert_code(x->dr, code, code_len - 1, ch);
    }
}

int16_t huffmanCompressor::find_reverse(hNode* x, int64_t code, int max_len)
{
    if(x == nullptr)
        throw runtime_error ("Searching code in huffman null node!\n");

    if(max_len == 0)    //if we reached end of input return stored char
        return x->ch;

    if(x->st == nullptr && x->dr == nullptr)    //if it's leaf node return stored char
        return x->ch;

    //else compute new code and
    bool bit = code & 1;

    //search down the tree
    if(bit == 0)
        return find_reverse(x->st, code >> 1, max_len - 1);
    else
        return find_reverse(x->dr, code >> 1, max_len - 1);
}

int64_t brev(int64_t x, int sz)
{
    int ans = 0;
    for(int i = 0; i < sz; ++i)
    {
        ans <<= 1;
        ans |= (x & 1);
        x >>= 1;
    }

    return ans;
}

int huffmanCompressor::compress(char * src, char * dst, int inputSize, int & outputSize)
{
    //stats
    inputsize += inputSize;

    outputSize = 0;

    for(int i = 0; i < inputSize; ++i)
    {
        //fetch character
        unsigned char c = (unsigned char)src[i];

        int codesize = table[c].code_len;
        int64_t code = brev(table[c].code, codesize);

        cout<<"Char "<<c<<", code len "<<table[c].code_len<<", code "<<table[c].code<<"\n";

        //if codesize is 0 but character appears something went wrong
        if(codesize == 0 && table[c].frequency != 0)
            throw runtime_error("Error: character has a code size of 0!\n");

        carry = carry | ((1uLL * code) << nr_carry_bits);
        //cout<<"Generated carry: 0x"<<hex<<carry<<dec<<"\n";

        //determine how many full bytes there are
        int full_bytes = (codesize + nr_carry_bits) / 8;
        nr_carry_bits = (codesize + nr_carry_bits) % 8;

        //cout<<"Full bytes: "<<full_bytes<<"\n";
        //cout<<"Leftover bits: "<<nr_carry_bits<<"\n";

        //copy the full bytes
        memcpy(dst + outputSize, &carry, full_bytes);
        outputSize += full_bytes;

        //stats
        outputsize += full_bytes;

        //save the rest
        carry = carry >> (8 * full_bytes);
    }

    cout<<"Compression done - "<<outputsize<<"\n";
    return 1;
}

int huffmanCompressor::compressFinal(char * src, char * dst, int inputSize, int & outputSize)
{
    //stats
    inputsize += inputSize;

    outputSize = 0;

    for(int i = 0; i <= inputSize; ++i)
    {
        if(i == inputSize)
        {
            int sz = (nr_carry_bits + 8) / 8;
            memcpy(dst + outputSize, &carry, sz); //pad with zero
            outputSize += sz;

            //stats
            outputsize += sz;
            break;
        }

        //fetch character
        unsigned char c = (unsigned char)src[i];

        int codesize = table[c].code_len;
        int64_t code = brev(table[c].code, codesize);

        cout<<"Char "<<c<<", code len "<<table[c].code_len<<", code "<<table[c].code<<"\n";

        //if codesize is 0 but character appears something went wrong
        if(codesize == 0 && table[c].frequency != 0)
            throw runtime_error("Error: character has a code size of 0!\n");

        carry = carry | ((1uLL * code) << nr_carry_bits);
        //cout<<"Generated carry: 0x"<<hex<<carry<<dec<<"\n";

        //determine how many full bytes there are
        int full_bytes = (codesize + nr_carry_bits) / 8;
        nr_carry_bits = (codesize + nr_carry_bits) % 8;

        //cout<<"Full bytes: "<<full_bytes<<"\n";
        //cout<<"Leftover bits: "<<nr_carry_bits<<"\n";

        //copy the full bytes
        memcpy(dst + outputSize, &carry, full_bytes);
        outputSize += full_bytes;

        //stats
        outputsize += full_bytes;

        //save the rest
        carry = carry >> (8 * full_bytes);
    }

    cout<<"Compression done - "<<outputsize<<"\n";
    return 1;
}

int huffmanCompressor::decompress(char * src, int inputSize, string & dst)
{
    //read from stream
    for(int i = 0; i < inputSize;)
    {
        int64_t nw = carry; //next input code is equal with what's left
        int full_bytes = 0; //number of full bytes read

        //cout<<hex<<"Carry is 0x"<<nw<<"\n";

        if(nr_carry_bits < 31)  //need to fetch more bits
        {
            //determine how many bytes to read
            full_bytes = min(4, inputSize - i);
            //cout<<"Reading extra "<<full_bytes<<" bytes\n";

            if(full_bytes)
            {
                //read from raw data stream
                memcpy(&carry, src + i, full_bytes);
                i += full_bytes;

                //cout<<"Read from input 0x"<<carry<<"\n";

                //complete the code with the new data
                nw = nw | (carry << nr_carry_bits);
            }
        }

        //cout<<"Final code 0x"<<nw<<"\n";

        //search for the code
        int16_t chi16 = find_reverse(root, nw, nr_carry_bits + full_bytes * 8);

        if(chi16 == -1)
        {
            cout<<"Warning! Code not found!\n";
            return i;
        }

        //extract char
        unsigned char ch = chi16 & 0xFF;

        //and append it to output

        dst += (char) ch;
        //cout<<"Decoded \""<<ch<<"\"";

        //get its code length
        int codelen = table[ch].code_len;
        //cout<<" with code length "<<codelen<<"\n";
        nw >>= codelen;

        //cout<<"Final carry 0x"<<nw<<"\n";

        //save bits
        carry = nw;
        nr_carry_bits = nr_carry_bits + full_bytes * 8 - codelen;
        //cout<<dec<<"Extra bits "<<nr_carry_bits<<"\n\n";

        //stats
        outputsize ++;
    }

    //stats
    inputsize += inputSize;

    return inputSize;
}

int huffmanCompressor::decompressFinal(char * src, int inputSize, string & dst)
{
    //read from stream
    for(int i = 0; i < inputSize || nr_carry_bits;)
    {
        int64_t nw = carry; //next input code is equal with what's left
        int full_bytes = 0; //number of full bytes read

        //cout<<hex<<"Carry is 0x"<<nw<<"\n";

        if(nr_carry_bits < 31)  //need to fetch more bits
        {
            //determine how many bytes to read
            full_bytes = min(4, inputSize - i);
            //cout<<"Reading extra "<<full_bytes<<" bytes\n";

            if(full_bytes)
            {
                //read from raw data stream
                memcpy(&carry, src + i, full_bytes);
                i += full_bytes;

                //cout<<"Read from input 0x"<<carry<<"\n";

                //complete the code with the new data
                nw = nw | (carry << nr_carry_bits);
            }
        }

        //cout<<"Final code 0x"<<nw<<"\n";

        //search for the code
        int16_t chi16 = find_reverse(root, nw, nr_carry_bits + full_bytes * 8);

        if(chi16 == -1)
        {
            if(i < inputSize)
                cout<<"Warning! Code not found!\n";
            return i;
        }

        //extract char
        unsigned char ch = chi16 & 0xFF;

        //and append it to output

        dst += (char) ch;
        //cout<<"Decoded \""<<ch<<"\"";

        //get its code length
        int codelen = table[ch].code_len;
        //cout<<" with code length "<<codelen<<"\n";
        nw >>= codelen;

        //cout<<"Final carry 0x"<<nw<<"\n";

        //save bits
        carry = nw;
        nr_carry_bits = nr_carry_bits + full_bytes * 8 - codelen;
        //cout<<dec<<"Extra bits "<<nr_carry_bits<<"\n\n";

        //stats
        outputsize ++;
    }

    //stats
    inputsize += inputSize;

    return inputSize;
}

int huffmanCompressor::serialize(char * dst)
{
    for(int i = 0; i < 256; ++i)
        dst[i] = table[i].code_len & 0xFF;

    return 256;
}

int huffmanCompressor::deserialize(char * src)
{
    //reset tree
    reset();

    //initialize table
    for(int i = 0; i < 256; ++i)
    {
        table[i].code_len = src[i];
        table[i].frequency = 1;
        table[i].ch = (unsigned char)i;
        table[i].code = 0;
    }

    //sort it in canonic order
    sort(table, table + 256, canonic_cmp);

    //generate codes
    int new_code = 0;
    int old_len = 0;
    for(int i = 0; i < 256; ++i)
    {
        if(table[i].code_len == 0)
            continue;
        new_code = new_code << (table[i].code_len - old_len);
        old_len = table[i].code_len;

        table[i].code = new_code;
        new_code++;
    }

    //make it alphabetic again
    sort(table, table + 256, alphabetic_cmp);

    cout<<"Debug print\n";
    for(int i = 0; i < 256; ++i)
        cout<<table[i].ch<<" "<<table[i].frequency<<" "<<table[i].code_len<<" "<<table[i].code<<"\n";

    //insert codes in trie
    for(int i = 0; i < 256; ++i)
        insert_code(root, table[i].code, table[i].code_len, table[i].ch);

    return 1;
}

void huffmanCompressor::reset()
{
    memset(Q1, 0, sizeof(Q1));

    root = Q1;
    root->ch = -1;
    q1 = 0;
    q2 = 0;

    actual_char_number = 0;

    tree_built = 0;

    inputsize = 0;
    outputsize = 0;

    carry = 0;
    nr_carry_bits = 0;
}

huffmanCompressor::~huffmanCompressor()
{
    delete [] table;
    delete [] links;

    table = nullptr;
    links = nullptr;
}
