//std
#include <iostream>
#include <string.h>
#include <vector>
//#include <set>
#include <algorithm>

//posix
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

//c
#include <stdlib.h>

using namespace std;

class node
{
    //voi face un alt arbore in care voi stoca detalii despre fiecare director si fisier
protected:
    int maxdepth;
    uint64_t sz; //dimensiune
    string name; //path

public:
    //getter/setter
    string get_name () const
    {
        return this->name;
    }

    uint64_t get_size() const
    {
        return this->sz;
    }

    int get_maxdepth() const
    {
        return this->maxdepth;
    }

    virtual uint64_t process() = 0;
    virtual void print(int level, int last, vector<bool> & ap, uint64_t p_size, uint32_t max_show) = 0;

    bool operator == (const node & rhs)
    {
        return this->name == rhs.name;
    }

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
        this->maxdepth = 1;
    }

    file_node(const char * new_name, uint64_t new_size)
    {
        this->name = "";
        this->name += new_name;
        this->sz = new_size;
        this->maxdepth = 1;
    }

    uint64_t process()
    {
        return this->sz;
    }

    void print(int lvl, int last, vector<bool> & ap, uint64_t p_size, uint32_t max_show)
    {
        for(int i = 1; i < lvl; ++i)
            if(ap[i])
                cout<<"| ";
            else
                cout<<"  ";

        double procent = 0;
        if(p_size == 0)
            procent = 100;
        else
        {
            procent = (double)this->sz / (double)(p_size);
            procent *= 100.;
        }

        if(last)
        {
            cout<<"\xC0\xC4"<<this->name<<" - "<<this->sz<<" ("<<procent<<" % of parent)\n";
            ap[lvl] = 0;    //nu mai afisez linii pe randu asta
        }
        else
            cout<<"\xC3\xC4"<<this->name<<" - "<<this->sz<<" ("<<procent<<" % of parent)\n";
    }
};

//struct cmp
//{
    bool cmp (const node * l, const node * r)// const
    {
        if(l->get_size() == r->get_size())
            return l->get_name() < r->get_name();
        return l->get_size() > r->get_size();
    }
//};

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
            this->maxdepth = max(this->maxdepth, f->get_maxdepth());
        }

        sort(this->fiu.begin(), this->fiu.end(), cmp);
        return this->sz;
    }

    void print(int lvl, int last, vector<bool> & ap, uint64_t p_size, uint32_t max_show)
    {
        uint32_t already_shown = 0;

        for(int i = 1; i < lvl; ++i)
            if(ap[i])
                cout<<"| ";
            else
                cout<<"  ";

        double procent = 0;
        if(p_size == 0)
            procent = 100;
        else
        {
            procent = (double)this->sz / (double)(p_size);
            procent *= 100.;
        }

        if(last)
        {
            cout<<"\xC0\xC4"<<this->name<<" - "<<this->sz<<" ("<<procent<<" % of parent)\n";
            ap[lvl] = 0;    //nu mai afisez linii pe randu asta
        }
        else
            cout<<"\xC3\xC4"<<this->name<<" - "<<this->sz<<" ("<<procent<<" % of parent)\n";

        for(auto it = fiu.begin(); already_shown < max_show && it != fiu.end(); ++it, ++already_shown)
        {
            if(next(it, 1) != fiu.end())
            {
                //afisez linii pe randu urmator
                ap[lvl + 1] = 1;
                (*it)->print(lvl + 1, 0, ap, this->sz, max_show);
            }
            else
            {
                (*it)->print(lvl + 1, 1, ap, this->sz, max_show);
            }
        }

        if(already_shown < fiu.size())
        {
            for(int i = 1; i <= lvl; ++i)
            if(ap[i])
                cout<<"| ";
            else
                cout<<"  ";
            cout<<"\xC0\xC4"<<" and "<<fiu.size() - already_shown<<" more entries!\n";
            ap[lvl] = 0;    //nu mai afisez linii pe randu asta
        }
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

    //max elements shown
    uint32_t max_show = 5;

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

    else if(argc < 3)   //avem argv[1]
    {
        path = argv[1];
    }

    else
    {
        path = argv[1];
        max_show = strtol(argv[3], NULL, 10);
    }

    //orice caz am fi ales putem sa incepem sa exploram
    cout<<"CWD: "<<path<<"\n\n";

    //creez arborele meu
    folder_node * root = new folder_node(path);

    //parsam propriu-zis
    parse(path, 1, root);

    //procesam
    root->process();

    //vector de apartii pt liniile pe care o sa le trag
    vector<bool> ap;
    ap.reserve(root->get_maxdepth() + 2);
    ap[1] = 1;

    //afisam
    root->print(1, 1, ap, 0, max_show);
    return 0;
}
