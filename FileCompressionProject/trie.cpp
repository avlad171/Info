#include "trie.h"
#include <iostream>

using namespace std;

trie::trie()
{
    //create root
    T = new tNode();

    //init params
    nrStoredWords = 0;
    last_start = T;
}

trie::~trie()
{
    delete_inner(T);
    T = nullptr;
    last_start = nullptr;
    //cout<<"Trie deletion done\n";
}

void trie::delete_inner(tNode* x)
{
    if(x == nullptr)
        return;

    for(int i = 0; i < 256; ++i)
        delete_inner(x->sons[i]);

    delete x;
}

void trie::insert_inner(tNode* x, const unsigned char* str, int sz)
{
    //failsafe
    if(!x)
        throw runtime_error("Fatal error! Inserting into null node!\n");

    if(sz == 0) //end of the word, mark
    {
        x->isWord = nrStoredWords++;
        return;
    }

    else    //more of the word remaining
    {
        unsigned char c = *str;
        //cout<<"[DEBUG]"<<(unsigned int)c<<"\n";
        if(x->sons[(unsigned int)c] && sz)  //we already have a word with this prefix
            insert_inner(x->sons[(unsigned int)c], str + 1, sz - 1);

        else    //need to create new node
        {
            x->sons[(unsigned int)c] = new tNode();
            insert_inner(x->sons[(unsigned int)c], str + 1, sz - 1);
        }

        return;
    }
}

void trie::print_inner(tNode* x)
{
    static string ans = "";

    //failsafe
    if(!x)
        throw runtime_error("Fatal error! Printing null node!\n");

    if(x->isWord)
        cout<<ans<<" - "<<x->isWord<<"\n";

    for(int i = 0; i < 256; ++i)
    {
        if(x->sons[i])
        {
            ans.push_back((char)i);
            print_inner(x->sons[i]);
            ans.pop_back();
        }
    }
}

int trie::find_inner(tNode* x, const unsigned char* str, int sz)
{
    if(!x)
        return 0;

    if(sz == 0)
    {
        return x->isWord;
    }
    //cout<<"[DEBUG]"<<(unsigned int)(*str)<<"\n";
    return find_inner(x->sons[(unsigned int)(*str)], str + 1, sz - 1);

}

void trie::insert(const char * str, int sz)
{
    if(sz > 0)
        insert_inner(T, (const unsigned char*)str, sz);
}

int trie::find(const char * str, int sz)
{
    if(sz > 0)
        return find_inner(T, (const unsigned char*)str, sz);
    else
        return -1;
}

int trie::find_incremental(const char next)
{
    unsigned char c = (unsigned char) next;
    if(last_start->sons[c])
    {
        last_start = last_start->sons[c];
        if(last_start->isWord != -1)
            return last_start->isWord;
        else
        {
            last_start = T;
            return -1;
        }
    }

    else
    {
        last_start = T;
        return -1;
    }
}

int trie::find_incremental_and_add(const char next)
{
    unsigned char c = (unsigned char) next;
    if(last_start->sons[c])
    {
        last_start = last_start->sons[c];
        if(last_start->isWord != -1)
            return last_start->isWord;
        else
        {
            last_start = T;
            return -1;
        }
    }

    else
    {
        last_start->sons[c] = new tNode();
        last_start->sons[c]->isWord = nrStoredWords++;
        last_start = T;
        return -1;
    }
}

void trie::print()
{
    print_inner(T);
}
