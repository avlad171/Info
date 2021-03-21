#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

#include <string.h>

class tNode
{
public:
    int isWord;
    tNode* sons [256];

    tNode()
    {
        isWord = 0;
        memset(sons, 0, 256 * sizeof(tNode*));
    }
};

class trie
{
    //state variables
    int nrStoredWords;
    tNode* T;

    //specific functions
    void insert_inner(tNode*, const unsigned char*, int);
    void print_inner(tNode*);
    int find_inner(tNode*, const unsigned char *, int);

public:
    //ctor
    trie();

    //dtor
    ~trie();

    //serialize

    //deserialize


    void insert(const char *, int);
    int find(const char *, int);
    void print();
};

#endif // TRIE_H_INCLUDED
