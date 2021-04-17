#include "token.h"
#include <iostream>

using namespace std;

token::token(string str)
{
    int sz = str.size();

    data = str;

    H1 = 0;
    H2 = 0;

    for(int i = 0; i < sz; ++i)
    {
        H1 = (127LL * H1 + str[i]) % 1000000007LL;
        H2 = (127LL * H2 + str[i]) % 1000000009LL;
    }
}

token::token(const token & other)
{
    this->H1 = other.H1;
    this->H2 = other.H2;

    this->data = other.data;
}

void swap(token& first, token& second)
{
    swap(first.H1, second.H1);
    swap(first.H2, second.H2);

    swap(first.data, second.data);
}

token& token::operator = (token other)
{
    swap(*this, other);
    return *this;
}

bool token::operator == (const token & other) const
{
    //cout<<"Actually comparing \""<<data<<"\" with \""<<other.data<<"\"...\n";
    //failsafe for empty string
    //if(data.size() == 0 && other.data.size() == 0)
    //    return 1;

    if(data.size() != other.data.size())
        return 0;

    if(H1 != other.H1)
        return 0;

    if(H2 != other.H2)
        return 0;

    if(data[0] != other.data[0])
        return 0;

        //cout<<"...and they are equal.\n";
    return 1;
}

ostream& operator << (ostream& os, const token& tk)
{
    os<<tk.data;
    return os;
}

istream& operator >> (istream& is, token& tk)
{
    is>>tk.data;

    tk.H1 = 0;
    tk.H2 = 0;

    for(int i = 0; i < tk.size(); ++i)
    {
        tk.H1 = (127LL * tk.H1 + tk.data[i]) % 1000000007LL;
        tk.H2 = (127LL * tk.H2 + tk.data[i]) % 1000000009LL;
    }

    return is;
}
