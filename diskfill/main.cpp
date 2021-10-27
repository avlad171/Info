//std
#include <iostream>
#include <string.h>
#include <vector>

//posix
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

class node
{
    //voi face un alt arbore in care voi stoca detalii despre fiecare director si fisier
protected:
    uint64_t sz; //dimensiune
    string name; //path

public:
    //getter/setter
    string get_name ()
    {
        return this->name;
    }

    uint64_t get_size()
    {
        return this->sz;
    }

    virtual uint64_t process() = 0;
    virtual void print(int level, int lastlevel) = 0;

    //dtor
    virtual ~node() {};
};

class file_node : public node
{
public:
    file_node (string & new_name, uint64_t new_size)
    {
        this->name = new_name;
        this->sz = new_size;
    }

    file_node(const char * new_name, uint64_t new_size)
    {
        this->name = "";
        this->name += new_name;
        this->sz = new_size;
    }

    uint64_t process()
    {
        return this->sz;
    }

    void print(int lvl, int lastlvl)
    {
        for(int i = 0; i < lvl - 1; ++i)
            cout<<"| ";
        cout<<"\xC3\xC4"<<this->name<<" - "<<this->sz<<" (TBD)\n";
    }
};

class folder_node : public node
{
    vector <node *> fiu;

public:
    //ctor
    folder_node(string & new_name)
    {
        this->name = new_name;
        this->sz = 0;
    }

    folder_node(const char * new_name)
    {
        this->name = new_name;
        this->sz = 0;
    }

    //inserat elemente
    void insert_new_folder(folder_node * f)
    {
        if(f != nullptr)
            fiu.push_back(f);
    }

    void insert_new_file(file_node * f)
    {
        if(f != nullptr)
            fiu.push_back(f);
    }


    uint64_t process()
    {
        for(node * f : fiu)
        {
            f->process();
            this->sz += f->get_size();
        }

        return this->sz;
    }

    void print(int lvl, int lastlvl)
    {
        for(int i = 0; i < lvl - 1; ++i)
            cout<<"| ";
        cout<<"\xC3\xC4"<<this->name<<" - "<<this->sz<<" (TBD)\n";

        for(uint32_t i = 0; i < fiu.size() ; ++i)
        {
            fiu[i]->print(lvl + 1, lvl);
        }
        //if(fiu.size())
        //    fiu[fiu.size() - 1]->print(lvl + 1, lvl);
    }

    //dtor
    ~folder_node()
    {
        for (node * f : fiu)
        {
            if(f != nullptr)
                delete f;
        }
    }
};

#define MAX_PATH 4100
/*void printEntry(const char * s, int lvl)
{
    for(int i = 0; i < lvl - 1; ++i)
        cout<<"| ";
    cout<<"\xC3\xC4"<<s;
}*/

int parse(string path, int lvl, node * n)
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

		//cerem informatii despre ce se afla aici
		struct stat istat;      //trebe musai struct in fara ca sa nu se confunde cu functia
		if(stat(newpath.c_str(), &istat) < 0)
		{
			perror(newpath.c_str());
			return -1;
		}

		//testez ca si in arborele construit de mine sa ma aflu pe director
        folder_node * c = dynamic_cast<folder_node*> (n);
        if(!c)
        {
            cout<<"Error! Internal representation data structure corrupted!\n";
            return -1;
        }

		//daca chestia ii director il parsam recursiv
		if(S_ISDIR(istat.st_mode))
		{
            folder_node * f = new folder_node(dirinfo->d_name);
            c->insert_new_folder(f);
			parse(newpath, lvl + 1, f);
		}

		//daca ii fisier ii aflam dimensiunea
		else if(S_ISREG(istat.st_mode))
		{
		    file_node * f = new file_node(dirinfo->d_name, istat.st_size);
		    c->insert_new_file(f);
		}

		//daca nu-i atunci afisam atentionare...
		else
		{
            cout<<"\""<<newpath<<"\" is not a file nor a folder! Skipping!\n";
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

    //creez arborele meu
    folder_node * root = new folder_node(path);

    //parsam propriu-zis
    parse(path, 1, root);

    //procesam
    root->process();

    //afisam
    root->print(1, 1);
    return 0;
}
