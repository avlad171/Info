Index file system - readme

This program, when given an input file formatted in <key><data> style, generates an index for that file using BTrees.
BTrees combine the properties and speed of balanced binary search trees with an optimal size for its nodes to facilitate
disk operations (reads/stores). The B Tree together with the formated data file forms a structure close to a B+ Tree.

As there is confusion related to BTree's size (some people call it order, some degree etc) I will refer to it by "order".
So I consider that a BTree of order t has in each node at most 2t-1 key-value pairs and 2t sons.

Implementation:

I used 2 classes to implement the tree and one class to implement a variable sized key.
The first class is the B Tree itself which is defined in btree.h and implemened in breee.cpp. It has the basic functionality
required for this project such as insert and search. It also contains the inner methods to parse the index file, create a new
tree with user-defined parameters and the core BTree algorithms. Those follow closely the ideas from "Introduction to algorithms" 
by Thomas H. Cormen.

The second class is the tree node class (named bNode) which is also defined in btree.h and implemented in btree.cpp.
It contains the methods to serialize and deserialize a BTree node to be written/read from disk. Each node is aware of
the btree order, key size and its position in the file. These parameters are necesary to dynamic memory management.

The third class is a simple dynamic array implementing a string. I did not want to use something like C++'s std::string
because i want to be able to hold null bytes in my keys and also I require precise control over its size. This class is defined
and implemented in "keytype.h".

The needed functionality:
createindex (datafile, recordsize, keysize),
addrecord (datafile, key, value),
searchrecord (datafile, key) and
updaterecord (datafile, key, value)
is implemented in the main program (main.cpp) except parts of the first one wich lets the BTree constructor to read the key 
and record size. All the available options are explained at runtime using a simple menu.

Notes:

For each "database" you can have at most 1 index file.
If the index file is somehow corrupted you need to delete it or the program will crash.
The keys are entered as ascii text, not binary data (althrough binary data is supported and the program may be extended to use
such keys).
The records are ascii text. No newline is required.
When creating an index from an existing database an additional message is displayed to keep track of the progress as in some situations
this operation can be quite slow.
For the big file (1M records) the program compiled with -O3 takes > 1 minute, depending on the disk usage.
If slow, turn off any cloud sync service while using it.
