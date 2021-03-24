#ifndef LZW_H_INCLUDED
#define LZW_H_INCLUDED

#include "trie.h"
#include <inttypes.h>
#include <unordered_map>
#include <algorithm>

class record
{
    unsigned char* data;
    int sz;

public:
    //ctors
    record(const char* src, int sz)
    {
        this->sz = sz;
        data = new unsigned char [sz + 1];
        memcpy(data, src, sz);
    };

    record()
    {
        this->sz = 0;
        data = nullptr;
    }

    record(const record & other)
    {
        sz = other.sz;
        data = new char[sz + 1];
        memcpy(data, other.data, sz);
    };

    //dtor
    ~record()
    {
        if(data)
            delete [] data;
    };

    //getters
    unsigned char * getData()
    {
        return data;
    };

    int size()
    {
        return sz;
    };

    //swap
    friend void swap(record& first, record& second)
    {
        std::swap(first.sz, second.sz);
        std::swap(first.data, second.data);
    }

    //operators
    record& operator = (record other)
    {
        swap(*this, other);
        return *this;
    }

    friend std::ostream& operator << (std::ostream& os, const record & key)
    {
        os.write(key.data, key.sz);
        return os;
    };
};

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
    int last_code;

    //trie dictionary, used for compression
    trie* T;

    //hashmap dictionary, used for decompression
    std::unordered_map<int, record*> H;

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
