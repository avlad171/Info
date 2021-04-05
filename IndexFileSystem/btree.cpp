#include "btree.h"
#include <iostream>
#include <algorithm>

using namespace std;

bNode::bNode(int order, int keysize, uint64_t fileOffset)
{
    //init parameters
    this->order = order;
    this->fileOffset = fileOffset;
    this->storedKeys = 0;
    this->isLeaf = 1;
    this->keysize = keysize;

    //init keys/sons
    sons = new uint64_t [2 * order + 1];
    memset(sons, 0, (2 * order + 1) * sizeof(uint64_t));

    //keys = new uint64_t [2 * order];
    //memset(keys, 0, 2 * order * sizeof(uint64_t));

    //initialization for keys
    keytype emptykey(keysize);

    //allocate keys
    keys = new keytype [2 * order];

    //init keys so that .c_str() does not return null
    for(int i = 0; i < 2 * order; ++i)
        keys[i] = emptykey;

    values = new uint64_t [2 * order];
    memset(values, 0, 2 * order * sizeof(uint64_t));
};

void bNode::load(fstream& file, uint64_t offset)
{
    static int cnt = 1;
    //cout<<"Load called "<<cnt++<<" times!\n";
    //seek cursor to offset
    file.seekg(offset);

    /*int useless;
    file.read((char *)&storedKeys, 4);
    file.read((char *)&order, 4);
    file.read((char *)&isLeaf, 4);
    file.read((char *)&useless, 4);

    for(int i = 0; i < 2 * order - 1; ++i)
    {
        file.read((char*)&keys[i], 8);
        file.read((char*)&values[i], 8);
    }

    for(int i = 0; i < 2 * order; ++i)
        file.read((char*)&sons[i], 8);*/

    //allocate buffer for the serialized data
    char * buf = new char[getNodeSize() + 8];
    file.read(buf, getNodeSize());
    int cursor = 0;

    //deserialize data
    memcpy(&storedKeys, buf + cursor, 4);
    cursor += 4;
    memcpy(&order, buf + cursor, 4);
    cursor += 4;
    memcpy(&isLeaf, buf + cursor, 4);
    cursor += 4;
    memcpy(&keysize, buf + cursor, 4);
    cursor += 4;

    for(int i = 0; i < 2 * order - 1; ++i)
    {
        //memcpy(&keys[i], buf + cursor, 8);
        keys[i].set(buf + cursor, keysize);
        cursor += keysize;

        memcpy(&values[i], buf + cursor, 8);
        cursor += 8;
    }

    for(int i = 0; i < 2 * order; ++i)
    {
        memcpy(&sons[i], buf + cursor, 8);
        cursor += 8;
    }

    delete [] buf;
}

void bNode::store(fstream& file, uint64_t offset)
{
    static int cnt = 1;
    //cout<<"Store called "<<cnt++<<" times\n";
    //a node is stored as:
    //4 bytes - nr keys
    //4 bytes - order
    //4 bytes - isLeaf
    //4 bytes - keysize
    //(8 + keysize) * (2*order - 1) bytes for stored key-value pairs
    //8 * (2*order ) bytes of pointers for sons

    //seek cursor to offset
    file.seekp(offset);

    /*file.write((const char*)&storedKeys, 4);
    file.write((const char*)&order, 4);
    file.write((const char*)&isLeaf, 4);
    file.write((const char*)&isLeaf, 4);    //unused

    for(int i = 0; i < 2 * order - 1; ++i)
    {
        file.write((const char*)&keys[i], 8);
        file.write((const char*)&values[i], 8);
    }

    for(int i = 0; i < 2 * order; ++i)
        file.write((const char*)&sons[i], 8);

    //compute padding
    int sz = 4 + 4 + 4 + 4 + 16 * (2 * order - 1) + 8 * (2 * order);
    //cout<<"Node size (in bytes): "<<sz<<"\n";
    int left = sz % 512;
    left = (left == 0 ? 0 : 512 - left);

    //cout<<"Padding: "<<left<<"\n";
    for(int i = 0; i < left; ++i)
        file.write("\x00", 1);*/

    char * buf = new char [getNodeSize() + 8];
    memset(buf, 0, getNodeSize());
    int cursor = 0;

    memcpy(buf + cursor, &storedKeys, 4);
    cursor += 4;
    memcpy(buf + cursor, &order, 4);
    cursor += 4;
    memcpy(buf + cursor, &isLeaf, 4);
    cursor += 4;
    memcpy(buf + cursor, &keysize, 4);
    cursor += 4;

    for(int i = 0; i < 2 * order - 1; ++i)
    {
        //memcpy(buf + cursor, &keys[i], 8);
        memcpy(buf + cursor, keys[i].c_str(), keysize);
        cursor += keysize;

        memcpy(buf + cursor, &values[i], 8);
        cursor += 8;
    }

    for(int i = 0; i < 2 * order; ++i)
    {
        memcpy(buf + cursor, &sons[i], 8);
        cursor += 8;
    }

    file.write(buf, getNodeSize());
    delete [] buf;
}

int bNode::getNodeSize()
{
    int ans = 4 + 4 + 4 + 4 + (8 + keysize) * (2 * order - 1) + 8 * (2 * order);
    //int left = ans % 512;
    //left = (left == 0 ? 0 : 512 - left);

    //ans += left;
    return ans;
}

int bNode::getNodeSize(int order, int keysize)
{
    int ans = 4 + 4 + 4 + 4 + (8 + keysize) * (2 * order - 1) + 8 * (2 * order);
    //int left = ans % 512;
   // left = (left == 0 ? 0 : 512 - left);

    //ans += left;
    return ans;
}

int64_t persistentBTree::bTreeSearch(bNode* root, keytype key)
{
    int i = 0;

    //find position
    for(; i < root->storedKeys && key > root->keys[i]; ++i);

    if(i < root->storedKeys && root->keys[i] == key)    //found in this node
        return root->values[i];

    else if(root->isLeaf)   //not found in this node and this node does not have children
        return -1;


    //need to search in sons

    //first load son from disk
    bNode* son = new bNode(order, keysize, 0);
    son->load(index, root->sons[i]);

    uint64_t ans = 0;
    //do actual search
    ans = bTreeSearch(son, key);

    delete son;
    return ans;
}

void persistentBTree::bTreeSplitChild(bNode* x, int i, bNode* y)   //splits the ith child of x (referenced by y)
{
    //create the new node
    bNode* z = new bNode(order, keysize, 512 + nrNodes * bNode::getNodeSize(order, keysize));
    nrNodes++;

    //retrieve reference to old node
    //bNode* y = x->sons[i];

    //copy leaf attribute
    z->isLeaf = y->isLeaf;

    //copy half the keys to new node
    z->storedKeys = order - 1;
    for(int j = 0; j < z->storedKeys; ++j)
    {
        z->keys[j] = y->keys[order + j];
        z->values[j] = y->values[order + j];
    }

    //copy half the sons to new node
    for(int j = 0; j < order; ++j)
        z->sons[j] = y->sons[order + j];


    //reset key counter for old node
    y->storedKeys = order - 1;

    //shift sons and insert new one in x
    for(int j = x->storedKeys; j > i; --j)
        x->sons[j + 1] = x->sons[j];
    x->sons[i + 1] = z->fileOffset;

    //shift keys and insert new one in x
    for(int j = x->storedKeys - 1; j >= i; --j)
    {
        x->keys[j + 1] = x->keys[j];
        x->values[j + 1] = x->values[j];
    }
    x->keys[i] = y->keys[order - 1];
    x->values[i] = y->values[order - 1];

    //increase current key count
    x->storedKeys++;

    //save x, y and z
    y->store(index, y->fileOffset);
    z->store(index, z->fileOffset);
    x->store(index, x->fileOffset);

    delete z;
}

void persistentBTree::bTreeInsertNonfull(bNode* x, keytype k, uint64_t val)
{
    int i = x->storedKeys - 1;

    if(x->isLeaf)
    {
        //find key's place
        for(; i >= 0 && k < x->keys[i]; --i)
        {
            x->keys[i + 1] = x->keys[i];
            x->values[i + 1] = x->values[i];
        }

        //insert key
        x->keys[i + 1] = k;
        x->values[i + 1] = val;
        x->storedKeys++;
        x->store(index, x->fileOffset);
    }

    else
    {
        //find key's place
        for(; i >= 0 && k < x->keys[i]; --i);
        i++;

        //load target node
        //bNode* y = x->sons[i];
        bNode* y = new bNode(order, keysize, x->sons[i]);
        y->load(index, x->sons[i]);

        //test wether target node is full
        if(y->storedKeys >= 2 * order - 1)
        {
            //if it is we must split it
            bTreeSplitChild(x, i, y);

            //test if splitting inserted a key smaller than ours
            if(k > x->keys[i])
            {
                //if it is, increase pointer
                ++i;

                //reload target node
                //y = x->sons[i];
                y->fileOffset = x->sons[i];
                y->load(index, x->sons[i]);
            }
        }
        //finally insert into non-full child
        bTreeInsertNonfull(y, k, val);
        delete y;
    }
}

void persistentBTree::insert(keytype key, uint64_t valuePtr)
{
    if(T->storedKeys >= 2 * order - 1)
    {
        //root full, create new root
        uint64_t newRootNodePtr = 512 + nrNodes * bNode::getNodeSize(order, keysize);
        bNode * s = new bNode (order, keysize, newRootNodePtr);
        nrNodes++;

        s->sons[0] = rootNodePtr;
        s->isLeaf = false;

        //save temp pointer to old root
        bNode* temp = T;

        //save new root
        T = s;

        //save new root offset
        rootNodePtr = newRootNodePtr;

        //split old root
        bTreeSplitChild(s, 0, temp);

        delete temp;

        //insert node
        bTreeInsertNonfull(s, key, valuePtr);
    }

    else
    {
        bTreeInsertNonfull(T, key, valuePtr);
    }
}

int64_t persistentBTree::search(keytype key)
{
    return bTreeSearch(T, key);
}

void persistentBTree::createRootNode()
{
    this->nrNodes = 0;
    this->T = new bNode(this->order, keysize, 512 + this->nrNodes * bNode::getNodeSize(order, keysize));
    this->nrNodes ++;
    this->rootNodePtr = 512;
}

void persistentBTree::writeHeader()
{
    //4 bytes - MAGIC .BTI
    //4 bytes - checksum TBA
    //4 bytes - btree order
    //4 bytes - key size
    //8 bytes - record size
    //8 bytes - root node offset
    //8 bytes - number of nodes
    //72 bytes - reserved
    //400 bytes - record file path
    //total - 512 bytes, 1 hard disk sector

    index.seekp(0);
    index.write(".BTI", 4);
    index.write("-TBA", 4);
    index.write((const char*)&(this->order), 4);
    index.write((const char*)&(this->keysize), 4);
    index.write((const char*)&(this->recordsize), 8);
    index.write((const char*)&(this->rootNodePtr), 8);
    index.write((const char*)&(this->nrNodes), 8);

    //unused space
    const int ct0 = 0;
    for(int i = 0; i < 18; ++i)
        index.write((const char*)&ct0, 4);

    //database file path
    index.write(dbname.c_str(), min((int)dbname.length(), 400));

    //pad until 400
    for(int i = 0; i < 400 - (int)dbname.size(); ++i)
        index.write("\x00", 1);

    //write root
    T->store(index, rootNodePtr);
}

void persistentBTree::readHeader()
{
    char magic[5] = {0, 0, 0, 0, 0};

    //set cursor to 0
    index.seekg(0);

    //read magic
    index.read(magic, 4);

    if(strcmp(magic, ".BTI") != 0)
    {
        throw runtime_error("Index file corrupted!\n");
    }

    //read useless stuff
    index.read(magic, 4);

    //read parameters
    index.read((char*)&(this->order), 4);
    index.read((char*)&(this->keysize), 4);
    index.read((char*)&(this->recordsize), 8);
    index.read((char*)&(this->rootNodePtr), 8);
    index.read((char*)&(this->nrNodes), 8);

    //TODO read name
}

void persistentBTree::printInfo()
{
    //notify user
    cout<<"[+] Index info:\n";
    cout<<"B-Tree order: "<<this->order<<"\n";
    cout<<"Keysize: "<<this->keysize<<"\n";
    cout<<"Recordsize: "<<this->recordsize<<"\n";
    cout<<"Root node offset: "<<this->rootNodePtr<<"\n";
    cout<<"B-Tree size (nodes): "<<this->nrNodes<<"\n";
}

void persistentBTree::inorder_inner(bNode* root, int level)
{
    if(level > 20)
    {
        cout<<"Warning! Inorder reached level > 20! Exiting to avoid stack overflow\n";
        return;
    }

    if(root == nullptr)
    {
        cout<<"Warning! Inorder called on null pointer!\n";
        return;
    }

    bNode* son = new bNode(order, keysize, 0);

    if(root->sons[0])
    {
        son->load(index, root->sons[0]);
        inorder_inner(son, level + 1);
    }

    for(int i = 0; i < root->storedKeys; ++i)
    {
        cout<<"Level "<<level<<", key "<<root->keys[i]<<", value "<<root->values[i]<<"\n";
        if(root->sons[i + 1])
        {
            son->load(index, root->sons[i + 1]);
            inorder_inner(son, level + 1);
        }
    }

    delete son;
}

persistentBTree::persistentBTree(string dbpath)
{
    //from the specification the index should have the same name with an .ndx appended
    string indexpath = dbpath + ".ndx";

    //open file
    index.open(indexpath, ios::in | ios::out | ios::binary);

    if(index.fail())    //if file does not exist try creating it
    {
        cout<<"Could not open index file! Creating new one.\n";

        //create it
        index.open(indexpath, ios::out);
        if(index.fail())
        {
            throw runtime_error("Unable to create new index file!\n");
        }
        index.close();

        //open it
        index.open(indexpath, ios::in | ios::out | ios::binary);
        if(index.fail())
        {
            throw runtime_error("Unable to open newly created index file!\n");
        }

        //read btree parameters
        cout<<"Enter BTree order: ";
        while(1)
        {
            cin>>this->order;
            if(this->order > 1)
                break;

            cout<<"B-Tree order must be greater than 1!\n";
        }

        cout<<"Enter key size: ";
        while(1)
        {
            cin>>this->keysize;
            if(this->keysize > 0)
                break;

            cout<<"Key size must be greater than 0!\n";
        }

        cout<<"Enter record size: ";
        while(1)
        {
            cin>>this->recordsize;
            if(this->recordsize > 0)
                break;

            cout<<"Key size must be greater than 0!\n";
        }

        cout<<"Computed node size "<<bNode::getNodeSize(order, keysize)<<"\n";

        //save a copy to the database name
        this->dbname = dbpath;

        //create root node
        createRootNode();

        //write header
        writeHeader();
    }

    else    //file already exists
    {
        //parse header and load root
        readHeader();

        T = new bNode (order, keysize, rootNodePtr);
        T->load(index, rootNodePtr);
    }

}

persistentBTree::~persistentBTree()
{
    //cout<<"[DEBUG] Dtor called\n";

    //save index header
    writeHeader();

    //delete root
    delete T;

    //close files
    index.close();
}
