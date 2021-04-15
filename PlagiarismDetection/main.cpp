#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "token.h"

using namespace std;

void printUsage()
{
    cout<<"Usage: ./prog -p <file1> <file2> for direct comparison or\n"
        "./prog -t <directory> to apply the algorithm between each pair of files in the directory\n";
}

int d[20005][20005];
vector <token> LCSfull(vector<token> & a, vector<token> & b)    //full version of the lcs, returns the full string
{
    uint32_t i;
    uint32_t j;

    for(i = 1; i <= a.size(); i++)
        for(j = 1; j <= b.size(); j++)
            if (a[i - 1] == b[j - 1])
            {
                //cout<<"i = "<<i<<", j = "<<j<<"\n";
                //cout<<"Compared \""<<a[i - 1]<<"\" ("<<a[i - 1].getH1()<<", "<<a[i - 1].getH2()<<") with \""<<b[j - 1]<<"\" ("<<b[j - 1].getH1()<<", "<<b[j - 1].getH2()<<") and they are equal.\n";
                d[i][j] = 1 + d[i - 1][j - 1];
            }
            else
                d[i][j] = max(d[i - 1][j], d[i][j - 1]);

    //get the length of lcs
    int lcsLen = d[a.size()][b.size()];

    //create space for lcs itself
    vector <token> ans (lcsLen);

    //debug
    /*for(int i = 0; i <= a.size(); i++)
    {
        for(int j = 0; j <= b.size(); j++)
        {
            cout<<d[i][j]<<" ";
        }
        cout<<"\n";
    }*/

    //compute lcs via backtracking
    --i;
    --j;
    while(i)
    {
        if(a[i - 1] == b[j - 1])
        {
            ans[lcsLen - 1] = a[i - 1];
            i--;
            j--;
            lcsLen--;
        }

        else
        {
            if(d[i - 1][j] < d[i][j - 1])
                j--;
            else
                i--;
        }
    }

    return ans;
}

/*vector <token> LCS(vector<token> a, vector<token> b)
{

}*/

vector <token> parseFile(char * path)
{
    ifstream f (path);
    if(f.fail())
    {
        string err = "Unable to open file \"";
        err += path;
        err += "\"";
        throw runtime_error(err);
    }

    vector <token> ans;

    while(1)
    {
        token temp;
        f>>temp;
        if(f.eof())
            break;
        ans.push_back(temp);
    }

    return ans;
}

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        printUsage();
        return 1;
    }

    if(!strcmp(argv[1], "-p"))  //pair mode
    {
        if(argc < 4)    //this mode needs an extra argument
        {
            cout<<"Too few arguments\n";
            return 1;
        }

        //actual code begins here
        vector<token> s1 = parseFile(argv[2]);
        vector<token> s2 = parseFile(argv[3]);

        vector<token> lcs = LCSfull(s1, s2);
        cout<<"The common string of both text files: \"";
        for(token & i : lcs)
            cout<<i<<" ";
        cout<<"\".\n";
        cout<<"Length of lcs is "<<lcs.size()<<" words.\n";

        cout<<"That is "<<((lcs.size() * 100)/((double)s1.size()))<<"% of text 1 and "<<((lcs.size() * 100)/((double)s2.size()))<<"% of text 2.\n";

    }

    else if(!strcmp(argv[1], "-t"))     //tabular mode
    {
        cout<<"Tab mode not done yet!\n";
    }

    else    //invalid argument
    {
        cout<<"Invalid argument\n";
        return 1;
    }

    return 0;
}
