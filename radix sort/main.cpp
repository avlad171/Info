#include <iostream>
#include <fstream>
#include <string.h>

#define RADIX_SIZE 8    //numarul de biti, de cifre, etc. Ganditi-va la bazele de numeratie
#define TOTAL_BYTES sizeof(int)

#define get_byte(x) ((x>>(byte * 8))&0xFF)  //macroinstructiune care ia a n-a cifra, al n-lea bit, depinde ce vreti

#define maxn 10000005

using namespace std;
unsigned int n, a, b , c;   //n- nr elementelor, (a,b,c) "seed" pentru RNG
unsigned int v [maxn];      //numerele care trebe sa fie sortate

void bucket_sort(unsigned int * A, unsigned int * B, int byte)  //sortam din A in B dupa byteul (criteriul) byte
{
    unsigned int counter[1<<RADIX_SIZE];
    unsigned int total, aux;
    memset(counter, 0, sizeof(counter));

    for(unsigned int i=0;i<n;++i)
        ++counter[get_byte(A[i])];  //vedem cate elemente au al n-lea byte k
    total = 0;

    for(unsigned int i = 0; i < 1<<RADIX_SIZE; ++i) //parurgem toate "cifrele" acestei baze de numeratie
    {
        aux = counter[i];
        counter[i] = total;
        total += aux;
    }

    for(unsigned int i = 0; i < n; ++i)
    {
        B[counter[get_byte(A[i])]] = A[i];
        counter[get_byte(A[i])]++;
    }

}
int main ()
{
    ifstream in ("radixsort.in");
    ofstream out ("radixsort.out");
    in>> n >> a >> b >> c;
    v[0]=b;
    for(unsigned int i = 1; i < n; ++i) //problema de pe infoarena prefera sa genereze datele de intrare cu un LCG (un RNG foarte prost, usor de hackuit ;)) asa ca trebuie sa le generam. Daca vrem sa folosim codul in orice aplicatie practica stergem asta
        v[i] = (1LL * a * v[i-1]%c + b) % c;

    unsigned int * secarray = new unsigned int [n+1];   //avem nevoie de un nou vector pentru ca spatiu auxiliar
                                                        //nici nu-i voie sa stricam originalu
    for (unsigned int i = 0; i < TOTAL_BYTES; i ++)     //cat timp avem cifre
    {
        if(i % 2 == 0)
            bucket_sort(v, secarray, i);// sortez dupa byteul i
        else
            bucket_sort(secarray, v, i);  //pun din primu in al 2-lea si din al 2-lea in primu pt a economisi spatiu
    }

    for(unsigned int i = 0; i < n;i+=10)
        out << v[i]<<" ";
    in.close();
    out.close();
    return 0;
}
