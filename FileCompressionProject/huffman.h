#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED

#include <inttypes.h>
#include <string>
#include <algorithm>

struct hNode
{
    int sum = 0;
    int16_t pos = -1;
    int16_t ch = 0;

    hNode * st = nullptr;
    hNode * dr = nullptr;
};

struct tableEntry
{
    int frequency = 0;
    unsigned char ch = 0;
    int16_t code_len = 0;
    int64_t code = 0;

    bool operator < (const tableEntry & other) const
    {
        if(frequency == other.frequency)
            return ch < other.ch;
        return frequency < other.frequency;
    }
};

class huffmanCompressor
{
    //stats
    uint64_t inputsize;
    uint64_t outputsize;

    //internals
    int nr_carry_bits;
    uint64_t carry;
    int tree_built;

    //table
    tableEntry* table;
    int* links;
    int actual_char_number;

    //2 queues and their pointers
    hNode Q1 [520];
    hNode *Q2 = Q1 + 260;
    int q1;
    int q2;

    //temp and inf (sentinel) node
    hNode temp;
    hNode inf;
    hNode * root;

    void DF(hNode*, int64_t, int16_t);
    void insert_code(hNode*, int64_t, int16_t, unsigned char);
    int16_t find_reverse(hNode*, int64_t, int);

public:
    //ctor
    huffmanCompressor();

    //dtor
    ~huffmanCompressor();

    //core
    int updateFrequency(char *, int);
    int buildTree();
    int compress(char * src, char * dst, int inputSize, int & outputSize);
    int compressFinal(char * src, char * dst, int inputSize, int & outputSize);
    int decompress(char * src, int inputSize, std::string & dst);
    int decompressFinal(char * src, int inputSize, std::string & dst);

    int deserialize(char * src);
    int serialize(char * dst);

    //getters
    int isInitialized()
    {
        return tree_built;
    };

    //reset
    void reset();

    //stats
    void printInfo();
};

#endif // HUFFMAN_H_INCLUDED
