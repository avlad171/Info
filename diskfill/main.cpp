//std
#include <iostream>
#include <string.h>

//posix
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

class node
{
    int sz;

public:

};

#define MAX_PATH 4100
void printEntry(const char * s, int lvl)
{
    for(int i = 0; i < lvl - 1; ++i)
        cout<<"| ";
    cout<<"\xC3\xC4"<<s;
}

int parse(string path, int lvl)
{
    //deschidem directoru' posix-style
    DIR* d = opendir(path.c_str());
	if(d == NULL)
	{
		perror(path.c_str());
		return -1;
	}

	//aici parsam propriu-zis
	dirent* dirinfo;
	while((dirinfo = readdir(d)) != NULL)       //cat timp mai avem chestii in director
	{
		//nu parsam . si ..
		if(!strcmp(dirinfo->d_name, ".") || !strcmp(dirinfo->d_name, ".."))
			continue;

        //construim calea noua
        string newpath = path;
		newpath += "/";
		newpath += dirinfo->d_name;

		//cout<<newpath<<" ";
		printEntry(dirinfo->d_name, lvl);

		//cerem informatii despre ce se afla aici
		struct stat istat;      //trebe musai struct in fara ca sa nu se confunde cu functia
		if(stat(newpath.c_str(), &istat) < 0)
		{
			perror(newpath.c_str());
			return -1;
		}

		//daca chestia ii director il parsam recursiv
		if(S_ISDIR(istat.st_mode))
		{
		    cout<<"\n";
			parse(newpath, lvl + 1);
		}

		//daca ii fisier ii aflam dimensiunea
		else if(S_ISREG(istat.st_mode))
		{
            cout<<" - "<<istat.st_size<<"\n";
		}

		//daca nu-i atunci afisam UNK sau ceva...
		else
		{
            cout<<" - UNK\n";
		}
	}

	//cleanup
	if(closedir(d) < 0)
	{
		perror("closedir()");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int main(int argc, char * argv[])
{
    //2 moduri: 1 - fara argumente, pleaca de la cwd
    //          2 - cu argumente, pleaca de la argv[1]

    //calea
    string path = "";

    if(argc < 2)
    {
        //obtinem cwd
        char buf[MAX_PATH];
        char * ans = getcwd(buf, MAX_PATH);

        if(!ans)    //eroare
        {
            perror("CWD error!: ");
            return -1;
        }

        path += ans;
        //aici avem cwd in path
    }

    else    //avem argv[1]
    {
        path = argv[1];
    }

    //orice caz am fi ales putem sa incepem sa exploram
    cout<<"CWD: "<<path<<"\n\n";


    //parsam propriu-zis
    return parse(path, 1);
}
