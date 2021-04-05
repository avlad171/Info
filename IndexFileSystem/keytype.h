#ifndef KEYTYPE_H_INCLUDED
#define KEYTYPE_H_INCLUDED

#include <iostream>

class keytype
{
    int sz;
    char * val;

public:
    //ctors
    keytype()
    {
        sz = 0;
        val = nullptr;
    };

    keytype(int sz)
    {
        this->sz = sz;
        val = new char [sz + 8];
        memset(val, 0, sz);
    };

    keytype(const char * src, int sz)
    {
        this->sz = sz;
        val = new char [sz + 8];
        memcpy(val, src, sz);
    };

    keytype(const keytype & other)
    {
        sz = other.sz;
        val = new char[sz + 8];
        memcpy(val, other.val, sz);
    };

    //dtor
    ~keytype()
    {
        delete [] val;
    }

    //getters
    const char * c_str() const  //TODO handle the empty string situation somehow
    {
        return (const char*)val;
    };

    int size() const
    {
        return sz;
    };

    //setter
    void set(const char * src, int sz)
    {
        if(this->sz != sz)
        {
            delete [] val;

            this->sz = sz;
            val = new char [sz + 8];
            memcpy(val, src, sz);
        }

        else
        {
            memcpy(val, src, sz);
        }

        //std::cout<<"[DEBUG] Source contains \""<<src<<"\", dst contains \""<<val<<"\"\n";
    }

    //swap
    friend void swap(keytype& first, keytype& second)
    {
        std::swap(first.sz, second.sz);
        std::swap(first.val, second.val);
    }

    //operators
    keytype& operator = (keytype other)
    {
        swap(*this, other);
        return *this;
    }

    bool operator < (const keytype & other) const
    {
        if(sz != other.sz)  //should not happen but whatever
        {
            std::cout<<"Warning: comparing (LT) keys of different sizes!\n";
            return 0;
        }

        return (memcmp(val, other.val, sz) < 0);
    };

    bool operator > (const keytype & other) const
    {
        if(sz != other.sz)  //should not happen but whatever
        {
            std::cout<<"Warning: comparing (GT) keys of different sizes!\n";
            return 0;
        }

        return (memcmp(val, other.val, sz) > 0);
    };

    bool operator == (const keytype & other) const
    {
        if(sz != other.sz)  //should not happen but whatever
        {
            std::cout<<"Warning: comparing (EQ) keys of different sizes!\n";
            return 0;
        }

        return (memcmp(val, other.val, sz) == 0);
    }

    bool operator != (const keytype & other) const
    {
        if(sz != other.sz)  //should not happen but whatever
        {
            std::cout<<"Warning: comparing (NE) keys of different sizes!\n";
            return 0;
        }

        return (memcmp(val, other.val, sz) != 0);
    };

    friend std::ostream& operator << (std::ostream& os, const keytype & key)
    {
        os.write(key.val, key.sz);
        return os;
    };
};

#endif // KEYTYPE_H_INCLUDED
