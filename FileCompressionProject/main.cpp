#include <iostream>
#include "trie.h"

using namespace std;

int main(int argc, char * argv[])
{
    trie T;
    /*T.insert("salam", 5);
    T.insert("sa", 2);
    T.insert("salala", 6);
    T.insert("abc", 3);
    T.insert("vand golf 4", 11);
    T.insert("vand golf 5", 11);
    T.print();

    cout<<T.find("sa", 2);
    cout<<T.find("sa", 2);
    cout<<T.find("sad", 3);
    cout<<T.find("sd", 2);
    cout<<T.find("abc", 3);
    cout<<T.find("abcd", 4);*/

    //LZW
    for (int i = 0; i < 256; ++i)
    {
        cout<<"Inserting "<<i<<"\n";
        T.insert((const char*)&i, 1);
    }

        cout<<"Insertion done\n";
    T.print();
    return 0;
}
