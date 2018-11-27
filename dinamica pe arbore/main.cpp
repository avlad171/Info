#include <iostream>
#include <fstream>
#include <vector>

#define maxn 10005

using namespace std;

vector <int> G[maxn];
int C[maxn];
int best[maxn]; ///cel mai bun scor pe nodul i
int best2[maxn]; //cel mai bun scor pe fii lu' i - cazul in care nu il includem pe i
int N, M, sol1, sol2;

void CF(int V, int pV)
{
    int sum1 = 0, sum2 = 0;
    for(auto v : G[V])
    {
        if(v == pV)
            continue;
        CF(v, V);
        sum1 += best2[v];
        sum2 += max(best[v], best2[v]);
    }

    best[V] = C[V] + sum1;
    best2[V] = sum2;
}

void DEFEU(int cur, int tata)
{
    int s1 = 0, s2 = 0;
    for(int vecin : G[cur])
    {
        if(vecin == tata)
            continue;
        DEFEU(vecin, cur);
        s1 += best[vecin];
        s2 += best2[vecin];
    }
    best2[cur] = s1;
    best[cur] = max(C[cur] + s2, s1);
}
int main()
{
    ifstream in ("data.txt"); ///datele urmeaza acest tipar: N, M apoi M de (x y) apoi C[i] reprezentand monedele fiecarui nod
    in>>N>>M;
    for(int i = 0; i < M; ++i)
    {
        int x, y;
        in>>x>>y;
        G[y].push_back(x);
        G[x].push_back(y);

    }
    for(int i = 1; i <= N; ++i)
        in>>C[i];

    DEFEU(1, 0);
    sol1 = best[1];
    for(int i = 0; i < maxn; ++i)
        best[i] = 0, best2[i] = 0;
    CF(1, 0);
    sol2 = max(best[1], best2[1]);
    cout<<sol1<<" "<<sol2<<"\n";
    return 0;
}
