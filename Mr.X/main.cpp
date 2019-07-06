#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

char op[2][4] = {"&|^", ""};
char query[1000];
char *p;

struct nod
{
    int val;   //0, 1 sunt valorile 0, 1, dar 2 si 3 sunt x respectiv !x(X)
    char opcode;
    nod *st, *dr;

    nod(int valoare = 0, char op = 0, nod * fiu_st = NULL, nod * fiu_dr = NULL)
    {
        val = valoare;
        opcode = op;
        st = fiu_st;
        dr = fiu_dr;
    }
};
nod * T;

nod * expr (int ordin)
{
    nod * x, * y;
    if(ordin == 1)
    {
        if(*p == '(')   //daca am gasit paranteza, facem un subarbore nou cu expresia din paranteza
        {
            ++p;
            x = expr(0);
            ++p;        //inchidem paranteza
        }
        else
        {
            x = new nod();
            char c = *p;
            ++p;

            if(c == '1')
                x->val = 1;
            if(c == '0')
                x->val = 0;
            if(c == 'x')
                x->val = 2;
            if(c == 'X')
                x->val = 3; //X este x negat
        }
    }
    else
    {
        x = expr(ordin+1);
        for(; strchr(op[ordin], *p); x = y)
            y = new nod(0, *p++, x, expr(ordin+1));
    }
    return x;
}

bool eval(nod *n, bool x)
{
    switch(n->opcode)
    {
        case '&':
            return eval(n->st, x) & eval(n->dr, x);
        case '|':
            return eval(n->st, x) | eval(n->dr, x);
        case '^':
            return eval(n->st, x) ^ eval(n->dr, x);
        default:
            if (x == false)
            {
                if(n->val == 2)
                    return false;
                else if(n->val == 3)
                    return true;
                else return n->val;
            }
            else
            {
                if(n->val == 2)
                    return true;
                else if(n->val == 3)
                    return false;
                else return n->val;
            }
    }
}

int main()
{
    int n;
    ifstream in ("mr_x_2.txt");
    ofstream out ("output.txt");

    in>>n;
    for(int i = 1; i <= n; ++i)
    {
        //cout<<"Iteratia "<<i<<"\n";
        in>>query;
        query[strlen(query)] = '.'; //terminator de sir, daca nu-l pun trece strchr peste lungimea maxima si face aiura
        query[strlen(query) + 1] = 0;
        p = query; //inceputu' sirului
        T = expr(0);


        if(eval(T, 0) == eval(T, 1))    //expresia nu depinde de x
        {
            out<<"Case #"<<i<<": 0\n";
            continue;
        }
        else
        {
            out<<"Case #"<<i<<": 1\n";
            continue;
        }
    }

    return 0;
}
