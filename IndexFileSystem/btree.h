#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <fstream>
#include <string>
#include <string.h>
#include "keytype.h"

class bNode
{
public:
    //parameters
    int storedKeys; //n[x] in cormen
    int order;
    int isLeaf;
    int keysize;
    uint64_t fileOffset;
    uint64_t * sons;
    //uint64_t * keys;
    keytype * keys;
    uint64_t * values;

    //ctor
    bNode(int order, int keysize, uint64_t fileOffset);

    //dtor
    ~bNode()
    {
        delete [] sons;
        delete [] keys;
        delete [] values;
    }

    //load/store
    void load(std::fstream& file, uint64_t offset);
    void store(std::fstream& file, uint64_t offset);

    //utility
    int getNodeSize();
    static int getNodeSize(int order, int keysize);
};

class persistentBTree
{
    //parameters
    int order;
    int keysize;
    uint64_t recordsize;
    uint64_t rootNodePtr;
    uint64_t nrNodes;
    uint64_t nodeSize;

    //b tree functions
    void createRootNode();
    void bTreeSplitChild (bNode*, int, bNode*);
    void bTreeInsertNonfull (bNode*, keytype, uint64_t);
    int64_t bTreeSearch (bNode*, keytype);


    //index file
    std::fstream index;
    void writeHeader();
    void readHeader();

    //record file
    std::string dbname;

    //root
    bNode* T;

    //utility
    void inorder_inner(bNode* root, int lvl);

public:
    //core functionality
    void insert(keytype key, uint64_t valuePtr);
    int64_t search(keytype key);
    //void delete(uint64_t key);

    //utility
    void printInfo();
    int getKeySize() { return keysize; };
    uint64_t getRecordSize() { return recordsize; };

    void inorder()
    {
        inorder_inner(T, 0);
    };

    //ctor
    persistentBTree(std::string path);

    //dtor
    ~persistentBTree();
};

#endif // BTREE_H_INCLUDED
