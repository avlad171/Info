#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

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
                d[i][j] = 1 + d[i - 1][j - 1];
            else
                d[i][j] = max(d[i - 1][j], d[i][j - 1]);

    //get the length of lcs
    int lcsLen = d[a.size()][b.size()];

    //create space for lcs itself
    vector <token> ans (lcsLen);

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
void NeedlemanWunsch(vector<token> & a, vector <token> & b)
{
    //init first row
    for(uint32_t i = 1; i <= b.size(); ++i)
        d[0][i] = -i;

    //dynamic prog
    for(uint32_t i = 1; i <= a.size(); ++i)
    {
        //init first collumn
        d[i][0] = -i;

        //actual computation
        for(uint32_t j = 1; j <= b.size(); ++j)
        {
            int del = d[i - 1][j] - 1;
            int ins = d[i][j - 1] - 1;

            int match = d[i - 1][j - 1] - 1;
            if(a[i - 1] == b[j - 1])
            {
                match = d[i - 1][j - 1];
            }

            d[i][j] = max(match, max(ins, del));
        }
    }

    //debug
    cout<<"DEBUG:\n";
    for(int i = 0; i <= a.size(); ++i)
    {
        for(int j = 0; j <= b.size(); ++j)
        {
            cout<<d[i][j]<<" ";
        }
        cout<<endl;
    }


}

vector<int> NWScore(vector<token> & a, vector<token> & b)
{
    //allocate mem
    int * _d = new int [2 * (b.size() + 2)];
    int * d [2];
    d[0] = _d;
    d[1] = _d + b.size() + 2;

    //set each element to 0
    memset(_d, 0, 2 * (b.size() + 2) * sizeof(int));

    //init first row
    for(uint32_t i = 1; i <= b.size(); ++i)
        d[0][i] = -i;

    //dynamic prog
    bool cLine = 1;
    for(uint32_t i = 1; i <= a.size(); ++i)
    {
        //init first collumn
        d[cLine][0] = -i;

        //actual computation
        for(uint32_t j = 1; j <= b.size(); ++j)
        {
            int del = d[cLine ^ 1][j] - 1;
            int ins = d[cLine][j - 1] - 1;

            int match = d[cLine ^ 1][j - 1] - 1;
            if(a[i - 1] == b[j - 1])
            {
                match = d[cLine ^ 1][j - 1];
            }

            d[cLine][j] = max(match, max(ins, del));
        }

        //switch current and previous lines
        cLine ^= 1;
    }

    //switch back to last line
    cLine ^= 1;

    //extract answer
    vector<int> ans;
    for(uint32_t i = 0; i <= b.size(); ++i)
        ans.push_back(d[cLine][i]);

    //cleanup and return
    delete [] _d;
    return ans;
}

vector<token> Hirschberg(vector<token> & a, vector<token> & b, int level)
{
    cout<<"HSB lvl "<<level<<"\na = ";
    for(auto & t : a)
        cout<<t<<" ";
    cout<<"\nb = ";
    for(auto & t : b)
        cout<<t<<" ";
    cout<<"\n";

    vector <token> lcs;

    //treat empty string
    if(a.size() == 0 || b.size() == 0)
        return lcs;

    //treat string with 1 element
    if(a.size() == 1)
    {
        if(find(b.begin(), b.end(), a[0]) != b.end())
        {
            lcs.push_back(a[0]);
        }
        return lcs;
    }

    if(b.size() == 1)
    {
        if(find(a.begin(), a.end(), b[0]) != a.end())
        {
            lcs.push_back(b[0]);
        }
        return lcs;
    }

    //treat other cases using divide et impera
    vector<int> scoreL;
    vector<int> scoreR;

    uint32_t xmid = a.size() / 2;
    cout<<"Computed xmid: "<<xmid<<"\n";

    vector<token> aL(a.begin(), a.begin() + xmid);
    vector<token> aR(a.begin() + xmid, a.end());

    //for the left side it's easy
    scoreL = NWScore(aL, b);

    //for the right side we need to reverse
    vector<token> b2(b);
    reverse(b2.begin(), b2.end());
    reverse(aR.begin(), aR.end());
    scoreR = NWScore(aR, b2);
    reverse(scoreR.begin(), scoreR.end());
    reverse(aR.begin(), aR.end());  //don't forget to reverse right half back to use it in recursive calls

    cout<<"Scores:\n";
    for(int i : scoreL)
        cout<<i<<" ";
    cout<<"\n";
    for(int i : scoreR)
        cout<<i<<" ";
    cout<<"\n";
    cout<<"\n";
    //compute y mid
    uint32_t ymid = 0;
    int maxim = 0x80000000;

    for(uint32_t i = 0; i <= b.size(); ++i)
        if(scoreL[i] + scoreR[i] >= maxim)
        {
            maxim = scoreL[i] + scoreR[i];
            ymid = i;
        }

    cout<<"Computed ymid: "<<ymid<<"\n";

    //divide the second string
    vector<token> bL(b.begin(), b.begin() + ymid);
    vector<token> bR(b.begin() + ymid, b.end());

    vector<token> lcs1 = Hirschberg(aL, bL, level + 1);
    vector<token> lcs2 = Hirschberg(aR, bR, level + 1);

    for(token & t : lcs1)
        lcs.push_back(t);

    for(token & t : lcs2)
        lcs.push_back(t);

    cout<<"End of level "<<level<<" routine!\n";
    return lcs;
}

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

        //NeedlemanWunsch(s1, s2);
        //vector<int> dsa = NWScore(s1, s2);
        //cout<<"NWScore is: "<<dsa[s2.size()]<<"\n";

        cout<<"\nHirschberg: ";
        vector<token> lcs2 = Hirschberg(s1, s2, 1);
        cout<<"The common string of both text files: \"";
        for(token & i : lcs2)
            cout<<i<<" ";
        cout<<"\".\n";
        cout<<"Length of lcs is "<<lcs2.size()<<" words.\n";
        cout<<"That is "<<((lcs2.size() * 100)/((double)s1.size()))<<"% of text 1 and "<<((lcs2.size() * 100)/((double)s2.size()))<<"% of text 2.\n";
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
