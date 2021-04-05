#include <iostream>
#include "btree.h"
#include "keytype.h"

using namespace std;

void showMenu()
{
    cout<<"Index File System\n";
    cout<<"Enter 1 <datafile> <recordsize> <keysize> to generate index for the <datafile> with given parameters\n";
    cout<<"Enter 2 <datafile> <key> <value> to add new record to file specified in <database>\n";
    cout<<"Enter 3 <datafile> <key> to search for the record with key <key> in <datafile>\n";
    cout<<"Enter 4 <datafile> <key> <value> to update record with key <key> in <datafile>\n";
    cout<<"Enter 5 <datafile> to print B-Tree info\n";
    cout<<"Enter 6 <datafile> to do an inorder traversal of the B-Tree\n";
    cout<<"Enter 0 to exit\n";
    cout<<"Your option: ";
}

int main()
{
    persistentBTree * BT = nullptr;

    int selection = 1;
    while(selection != 0)
    {
        showMenu();
        cin>>selection;

        switch (selection)
        {
        case 0:
            return 0;

        case 1:
            {
                keytype key;
                uint64_t cursor = 0;
                uint64_t recordsize;
                uint64_t keysize;
                string dbname = "";

                //read input data
                cout<<"Filename: ";
                cin>>dbname;

                fstream datafile (dbname, ios::in | ios::out | ios::binary);
                if(datafile.fail())
                {
                    cout<<"Unable to access datafile!\n";
                    break;
                }

                //init btree
                BT = new persistentBTree(dbname);

                recordsize = BT->getRecordSize();
                keysize = BT->getKeySize();

                //allocate buffer
                char* buf = new char [keysize + 1];

                int i = 0;
                while(!datafile.eof() && datafile.tellg() != -1)
                {
                    //read key
                    datafile.read(buf, keysize);
                    key.set(buf, keysize);

                    //skip over record
                    datafile.seekg(cursor + keysize + recordsize);

                    if(datafile.eof() || datafile.tellg() == -1)
                        break;

                    BT->insert(key, cursor);

                    if(i % 10000 == 0)
                    {
                        cout<<"[+]Processed "<<i<<"th entry\n";
                    }
                    ++i;

                    cursor += keysize;
                    cursor += recordsize;
                }

                BT->printInfo();

                //cleanup
                delete [] buf;
                delete BT;
                BT = nullptr;
                datafile.close();
            }
            break;

        case 2:
            {
                //read database name
                string dbname = "";
                cout<<"Filename: ";
                cin>>dbname;

                //generate index name
                string indexname = dbname + ".ndx";

                //test index
                ifstream test (indexname);
                if(test.fail())
                {
                    cout<<"No index exists for that datafile!\n";
                    break;
                }
                test.close();

                //open datafile
                fstream datafile(dbname, ios::in | ios::out | ios::binary | ios::ate);
                if(datafile.fail())
                {
                    cout<<"Fatal error! Unable to open datafile!\n";
                    break;
                }

                //get pointer to end of file
                uint64_t value;
                value = datafile.tellg();

                //init btree
                BT = new persistentBTree(dbname);

                //retrieve info
                int keysize = BT->getKeySize();
                uint64_t recordsize = BT->getRecordSize();

                //enter key to be searched
                keytype key;
                cout<<"Enter key for new record (max "<<keysize<<" bytes): ";
                string buf2 (keysize, 0);
                cin>>buf2;
                cin.get(); //skip newline
                key.set(buf2.c_str(), keysize);

                //search for key
                if(BT->search(key) != -1)
                {
                    cout<<"Error: key already exists!\n";
                    break;
                }

                //read record data
                cout<<"Enter new record data (max "<<recordsize - 1<<" characters): ";
                string record (recordsize, 0);
                getline(cin, record);

                //insert
                BT->insert(key, value);

                //write to file
                datafile.write(key.c_str(), keysize);
                datafile.write(record.c_str(), recordsize);

                //print some info
                cout<<"B-Tree status:\n";
                BT->printInfo();

                //clean
                datafile.close();
                delete BT;
                BT = nullptr;
            }
            break;

        case 3:
            {
                //read database name
                string dbname = "";
                cout<<"Filename: ";
                cin>>dbname;

                //generate index name
                string indexname = dbname + ".ndx";

                //test index
                ifstream test (indexname);
                if(test.fail())
                {
                    cout<<"No index exists for that datafile!\n";
                    break;
                }
                test.close();

                BT = new persistentBTree(dbname);
                int keysize = BT->getKeySize();

                //enter key to be searched
                keytype key;
                cout<<"Key to search (max "<<keysize<<" bytes): ";
                string buf2 (keysize, 0);
                cin>>buf2;
                key.set(buf2.c_str(), keysize);

                int64_t res = BT->search(key);

                if(res == -1)
                    cout<<"Key not found!\n";
                else
                {
                    cout<<"Value bound to key \""<<key<<"\": "<<res<<"\n";

                    ifstream datafile(dbname);
                    if(datafile.fail())
                    {
                        cout<<"Fatal error! Unable to open datafile!\n";
                        break;
                    }

                    cout<<"At that offset in the database file is the following record: ";

                    int keysize = BT->getKeySize();
                    uint64_t recordsize = BT->getRecordSize();

                    char* buf = new char [keysize + recordsize + 8];

                    datafile.seekg(res);
                    datafile.read(buf, keysize + recordsize);

                    cout<<buf<<"\n";

                    datafile.close();

                    delete [] buf;
                }

                delete BT;
                BT = nullptr;
            }
            break;

        case 4:
            {
                //read database name
                string dbname = "";
                cout<<"Filename: ";
                cin>>dbname;

                //generate index name
                string indexname = dbname + ".ndx";

                //test index
                ifstream test (indexname);
                if(test.fail())
                {
                    cout<<"No index exists for that datafile!\n";
                    break;
                }
                test.close();

                //open datafile
                fstream datafile(dbname, ios::in | ios::out | ios::binary);
                if(datafile.fail())
                {
                    cout<<"Fatal error! Unable to open datafile!\n";
                    break;
                }

                //init btree
                BT = new persistentBTree(dbname);

                //retrieve info
                int keysize = BT->getKeySize();
                uint64_t recordsize = BT->getRecordSize();

                //enter key to be searched
                keytype key;
                cout<<"Key to search (max "<<keysize<<" bytes): ";
                string buf2(keysize, 0);
                cin>>buf2;
                cin.get();  //skip over newline
                key.set(buf2.c_str(), keysize);

                int64_t res = BT->search(key);

                if(res == -1)
                {
                    cout<<"Key not found!\n";
                    break;
                }

                cout<<"The record with key "<<key<<" is in the datafile at offset "<<res<<"\n";

                //read record data
                cout<<"Enter new record data (max "<<recordsize - 1<<" characters): ";
                string record (recordsize, 0);
                getline(cin, record);

                //set file cursor to its position
                datafile.seekp(res + keysize);

                //write to file
                datafile.write(record.c_str(), recordsize);

                //clean
                datafile.close();
                delete BT;
                BT = nullptr;
            }
            break;

        case 5:
            {
                string dbname = "";
                cout<<"Filename: ";
                cin>>dbname;

                //generate index
                string indexname = dbname + ".ndx";

                //test index
                ifstream test (indexname);
                if(test.fail())
                {
                    cout<<"No index exists for that datafile!\n";
                    break;
                }
                test.close();

                BT = new persistentBTree(dbname);
                BT->printInfo();
                delete BT;
                BT = nullptr;
            }
            break;

        case 6:
            {
                string dbname = "";
                cout<<"Filename: ";
                cin>>dbname;

                //generate index
                string indexname = dbname + ".ndx";

                //test index
                ifstream test (indexname);
                if(test.fail())
                {
                    cout<<"No index exists for that datafile!\n";
                    break;
                }
                test.close();

                BT = new persistentBTree(dbname);
                BT->inorder();
                delete BT;
                BT = nullptr;
            }
            break;

        default:
            cout<<"Invalid option!\n";
        }

        cout<<endl;
    }
    return 0;
}
