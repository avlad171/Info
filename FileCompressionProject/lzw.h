#ifndef LZW_H_INCLUDED
#define LZW_H_INCLUDED

#include "trie.h"
#include <inttypes.h>
#include <unordered_map>
#include <algorithm>

//namespace lzw
//{
    typedef std::basic_string<unsigned char> bstring;
//}

class lzw
{
    unsigned char * buf;

    int codesize;

    void init();
    //stats
    uint64_t inputsize;
    uint64_t outputsize;

    //internals
    int nr_carry_bits;
    uint64_t carry;

    //internals - compressing
    int tStoredWords;
    int last_code;

    //internals - decompressing
    int hStoredWords;
    bstring old;

    //trie dictionary, used for compression
    trie* T;

    //hashmap dictionary, used for decompression
    std::unordered_map<int, bstring> H;

public:
    lzw(int sz);
    ~lzw();

    //core
    int compress(char * src, char * dst, int inputSize, int & outputSize);
    int compressFinal(char * src, char * dst, int inputSize, int & outputSize);
    int decompress(char * src, char * dst, int inputSize, int & outputSize);
    int decompressFinal(char * src, char * dst, int inputSize, int & outputSize);

    //getters
    getCodeSize()
    {
        return codesize;
    };
};

#endif // LZW_H_INCLUDED
