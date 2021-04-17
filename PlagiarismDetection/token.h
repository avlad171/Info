#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

class token
{
    //hashes for string matching
    int64_t H1;
    int64_t H2;

    //int64_t computeHash1();
    //int64_t computeHash2();

    std::string data;

public:
    token()
    {
        data = "";
        H1 = 0;
        H2 = 0;
    }

    token(std::string str);
    token(const token & other);

    //getters
    int size()
    {
        return data.size();
    };

    int64_t getH1()
    {
        return H1;
    }

    int64_t getH2()
    {
        return H2;
    }

    //operators
    friend void swap(token& first, token& second);

    token & operator = (token other);

    bool operator == (const token & other) const;

    friend std::ostream& operator << (std::ostream& os, const token & tk);

    friend std::istream& operator >> (std::istream& is, token & tk);
};

#endif // TOKEN_H_INCLUDED
