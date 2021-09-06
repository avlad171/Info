#include <iostream>

using namespace std;

struct complex
{
	double real;
	double imaginar;
};

ostream& operator << (ostream &os, const complex c)
{
    os<<c.real<<"+"<<c.imaginar<<"i";
    return os;
}

int main()
{
	int a = 2;
	complex c;
	c.real = -1.4;
	c.imaginar = 2.4;
	
	cout<<"Acesta este un nr intreg: "<<a<<" si acesta este unul complex: "<<c<<endl;
	
	return 0;
}