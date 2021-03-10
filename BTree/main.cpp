#include <iostream>
#include <time.h>
#include <string.h>

#include <fstream>
#include <string>
#include <queue>

using namespace std;

int order = 3;  //TODO do not hardcode

class bNode
{
public:
    int storedKeys; //n[x] in cormen
    bool isLeaf;
    bNode ** sons;
    int * keys;

    bNode(int order)
    {
        //cout<<"[DEBUG] Creating new B-Tree node!\n";
        sons = new bNode * [2 * order + 1];
        memset(sons, 0, (2 * order + 1) * sizeof(bNode*));

        keys = new int [2 * order];
        memset(keys, 0, 2 * order * sizeof(int));

        storedKeys = 0;
        isLeaf = 1;
    };

    //TODO add destructor
};

bNode * T;
void inorder(bNode * root, int level);

void bTreeSplitChild(bNode* x, int i)   //splits the ith child of x
{
    //create the new node
    bNode* z = new bNode(order);

    //retrieve reference to old node
    bNode* y = x->sons[i];

    //copy leaf attribute
    z->isLeaf = y->isLeaf;

    //copy half the keys to new node
    z->storedKeys = order - 1;
    for(int j = 0; j < z->storedKeys; ++j)
        z->keys[j] = y->keys[order + j];


    //copy half the sons to new node
    for(int j = 0; j < order; ++j)
        z->sons[j] = y->sons[order + j];


    //reset key counter for old node
    y->storedKeys = order - 1;

    //shift sons and insert new one in x
    for(int j = x->storedKeys; j > i; --j)
        x->sons[j + 1] = x->sons[j];
    x->sons[i + 1] = z;

    //shift keys and insert new one in x
    for(int j = x->storedKeys - 1; j >= i; --j)
        x->keys[j + 1] = x->keys[j];
    x->keys[i] = y->keys[order - 1];

    //increase current key count
    x->storedKeys++;
}

void bTreeInsertNonfull(bNode* x, int k)
{

    int i = x->storedKeys - 1;

    if(x->isLeaf)
    {
        //find key's place
        for(; i >= 0 && k < x->keys[i]; --i)
            x->keys[i + 1] = x->keys[i];

        //insert key
        x->keys[i + 1] = k;
        x->storedKeys++;
    }

    else
    {
        //find key's place
        for(; i >= 0 && k < x->keys[i]; --i);
        i++;

        //load target node
        bNode* y = x->sons[i];

        //test wether target node is full
        if(y->storedKeys >= 2 * order - 1)
        {
            //if it is we must split it
            bTreeSplitChild(x, i);

            //test if splitting inserted a key smaller than ours
            if(k > x->keys[i])
            {
                //if it is, increase pointer
                ++i;

                //reload target node
                y = x->sons[i];
            }
        }
        //finally insert into non-full child
        bTreeInsertNonfull(y, k);
    }
}

void bTreeInsert(bNode ** root, int k)
{
    //retrieve root
    bNode* r = *root;

    if(r->storedKeys >= 2 * order - 1)
    {
       // cout<<"\nRoot is full! Creating a new node!\n";
        //root full, create new root
        bNode * s = new bNode (order);
        s->sons[0] = r;
        s->isLeaf = false;

        //save new root
        *root = s;

        //split old root
        bTreeSplitChild(s, 0);

        //insert node
        bTreeInsertNonfull(s, k);
    }
    else
    {
        //cout<<"Root is not full, inserting "<<k<<"\n";
        bTreeInsertNonfull(r, k);
    }
}

void inorder(bNode * root, int level)
{
    if(level > 20)
    {
        cout<<"Warning! Inorder reached level > 20! Exiting to avoid stack overflow\n";
        return;
    }

    if(root == nullptr)
        return;

    inorder(root->sons[0], level + 1);

    for(int i = 0; i < root->storedKeys; ++i)
    {
        cout<<"Level "<<level<<", key "<<root->keys[i]<<"\n";
        inorder(root->sons[i + 1], level + 1);
    }
}

void BF(bNode* root)
{
    static int callCount = 0;

    ofstream render("render.dot");

    render<<"digraph btree{\n";
    //init BF
    int nrOrdine = 0;
    queue <pair<bNode*, int> > Q;

    Q.push(make_pair(root, nrOrdine++));
    render<<"n0 [shape = circle, label = n0]\n";
    //render<<"nullNode [shape = diamond, label = NULL, style = filled, fillcolor = red ]\n";

    while(!Q.empty())
    {
        bNode* currentNode = Q.front().first;
        int id = Q.front().second;
        Q.pop();

        //generate node in dot syntax
        render<<"n"<<id<<" [shape = record, label = \"";
        for(int i = 0; i < currentNode->storedKeys; ++i)
            render<<"<f"<<i<<"> |"<<currentNode->keys[i]<<"|";
        render<<"<f"<<currentNode->storedKeys<<">\"]\n";

        for(int i = 0; i <= currentNode->storedKeys; ++i)
        {
            int sonId = nrOrdine++;

            //treat leaves separately
            if(currentNode->isLeaf)
                continue;

            //error checking
            if(currentNode->sons[i] == NULL)
            {
                cout<<"Can not insert null son!\n";
                continue;
            }

            Q.push(make_pair(currentNode->sons[i], sonId));

            //declare edge
            render<<"\"n"<<id<<"\":f"<<i<<" -> n"<<sonId<<"\n";
        }
    }

    render<<"}\n";

    render.close();

    string command = "\"C:\\Program Files (x86)\\Graphviz2.38\\bin\\dot.exe\"";
    command += " -Tpng render.dot -o render" + to_string(callCount++) + ".png";

    system(command.c_str());
}

void preorder(bNode* root, int level)
{
    for(int i = 0; i < root->storedKeys; ++i)
        cout<<"Level "<<level<<", key "<<root->keys[i]<<"\n";
    cout<<"\n";

    for(int i = 0; i <= root->storedKeys; ++i)
    {
        if(root->sons[i])
            preorder(root->sons[i], level + 1);
        else
            cout<<"NULL\n";
    }
}
int main()
{
    cout << "B-Tree test!" << endl;

    srand(time(0));

    T = new bNode(order);

    /*for(int i = 0; i < 130; ++i)
    {
        int x = rand() % 120;
        cout<<"Inserting "<<x<<"\n";
        bTreeInsert(&T, x);
        if(i && (i % 3 == 0))
        {
            //cout<<"\n\nCalling inorder\n";
            //inorder(T, 0);
        }
    }*/

    /*for(int i = 1; i < 8000; ++i)
        {
            bTreeInsert(&T, rand());
        }*/

    bTreeInsert(&T, 35);
    BF(T);
    bTreeInsert(&T, 2);
    BF(T);
    bTreeInsert(&T, 1);
    BF(T);
    bTreeInsert(&T, 89);
    BF(T);
    bTreeInsert(&T, 4);
    BF(T);
    bTreeInsert(&T, 33);
    BF(T);
    bTreeInsert(&T, 24);
    BF(T);
    bTreeInsert(&T, 6);
    BF(T);
    bTreeInsert(&T, 7);
    BF(T);
    bTreeInsert(&T, 88);
    BF(T);
    bTreeInsert(&T, 14);
    BF(T);
    bTreeInsert(&T, 23);
    BF(T);
    bTreeInsert(&T, 22);
    BF(T);

    /*for(int i = 1; i <= 16; ++i)
    {
        bTreeInsert(&T, i);
        BF(T);
    }*/



    cout<<"\n\nFinal inorder\n";
    inorder(T, 0);



    return 0;
}
