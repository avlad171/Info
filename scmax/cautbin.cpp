#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
int n;
vector <pair<int, int> > a(100002), d(100002);      ///d[i] - ultimul element al lisului cu i elemente;
int lis;
int prv[100002];

ifstream in;
ofstream out;

int afis(int x)
{
    if (prv[x]) ///are precedent
        afis(prv[x]);
    out<<a[x].first<<" ";
}
int cautbin(int nr, int b, int e)
{
    int mid = (b + e)/2;
    while(b < e)
    {
    mid = (b + e)/2;
        if(d[mid].first >= nr)
            e = mid;
        else
            b = mid + 1;
    }
    return b;
}

int main()
{
    in.open("scmax.in");
    out.open("scmax.out");
    int lo;
    in>>n;

    for(int i = 1; i <= n; ++i)
    {
        in>>a[i].first;
        a[i].second = i;
    }

    lis = 1;    ///lungimea maxima a subsirului
    d[lis] = a[1];

    for (int i = 2; i <= n; ++i)
    {
        if(a[i].first > d[lis].first)
		{
		    ++lis;
			d[lis] = a[i];
			prv[a[i].second] = d[lis-1].second;
		}
		else
		{
        //    cout<<"Caut in 1, "<<lis<<"...\n";
		    lo = cautbin(a[i].first, 1, lis);
		    d[lo] = a[i];
		    prv[a[i].second] = d[lo-1].second;
		//    cout<<"Pun pe d("<<lo<<") "<<a[i]<<"\n";
		}
		//for (int j=1; j <= 6; ++j)
        //    cout<<d[j]<<" ";
        //cout<<"\n";
    }
    //for (int j=1; j <= 6; ++j)
    //    cout<<d[j]<<" ";
    //cout<<"\n";
    //cout<<lis<<"\n";
    //for(int i=2;i<=lis;++i)
    //    cout<<poz[i]<<" ";
    //cout<<d[lis]<<" ";
    cout<<"afisez";
    for(int i = 1; i <= n; ++i)
        cout<<prv[i]<<" ";
    out<<lis<<"\n";
    afis(d[lis].second);
    //for(int i = 2; i <= lis; ++i)
    //    out<<poz[i]<<" ";
    //out<<d[lis]<<" ";
    return 0;
}
