///Problema subsirului crescator de lungime maxima (LIS) - scmax pe infoarena
///Complexitatea O(n^2), obtine 70p

///compilam cu g++ -std=c++11 -O2 scmax.cpp -o scmax.exe sau trage sursa intr-un IDE
///librarii linkuite: niciuna
///dll-uri necesare: ale mingw-ului sau linkuie cu -static

#include <fstream>
#include <iostream>

#define maxn 100005

using namespace std;

int N;  //numarul de elem
int nr[maxn], best[maxn], pos[maxn];    //nr - sirul de numere, best = dinamica, pos = pozitia predecesorului

int main()
{
    int j, maxim = 1, p;

    ifstream in ("scmax.in");
    ofstream out ("scmax.out");
    in>>N;
    //citire
    for(int i = 1; i <= N; ++i)
        in>>nr[i];
    //initializez capatul
    best[N] = 1;
    pos[N] = -1; //nu avem predecesor

    //ma mut cu un indice mai in spate si ii parcurg pe toti cei din fata. daca ii mai mic actualizez besturile
    for(int i = N - 1; i > 0; --i)
    {
        best[i] = 1;
        pos[i] = -1;
        for(int j = i + 1; j <= N; ++j) //asta controleaza trecerea peste elem din fata
            if(nr[i] < nr[j] && best[i] < best[j] + 1)
            {
                best[i] = best[j]+1;
                pos[i] = j;

                if(best[i] > maxim)//daca cumva am modificat un best[i] si e mai mare decat maximul ar trebui actualizat
                {
                    maxim = best[i];
                    p = i;
                }
            }
    }

    out<<maxim<<"\n";
    int i = p;
    //pos e acuma o matrice a tatilor deci o putem parcurge in acelasi fel

    while(i!=-1)
    {
        out<<nr[i]<<" ";
        i = pos[i];
    }
    return 0;
}
