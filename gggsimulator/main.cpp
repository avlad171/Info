///DARKORBIT GALAXY GATE SPINS SIMULATOR


#include <iostream>
#include <time.h>
#include <windows.h>
#include <vector>
#include <time.h>

using namespace std;
bool gate [130];
int parts;
bool trigger = 1;

int simulate()
{

    memset(gate, 0, sizeof(gate));
    bool built = 0, multiplier = 0; //multiplic mereu la 2
    int nrspins = 0;
    while(!built)
    {
        int spin = rand() % 96;
        ++nrspins;
        if(spin < 13)
        {
            if(multiplier)   //a picat parte
            {
                int tobuild = 2;
                for(int i = 1; i <= parts && tobuild > 0; ++i)
                    if(!gate[i])
                    {
                        gate[i] = 1;
                        --tobuild;
                    }
            }
            else
            {
                int cur = rand() % parts + 1;
                if(!gate[cur])
                    gate[cur] = 1;
                else
                    multiplier = 1;
            }
        }
        else
            if(multiplier)
                multiplier = 0;

        built = 1;
        for(int i = 1; i <= parts; ++i)
            if(!gate[i])
                built = 0;
    }
    if(trigger)
    {
    cout<<"BUILT A GATE WITH "<<parts<<" PARTS USING "<<nrspins<<" SPINS!\n";
    cout<<"TOTAL COST: "<<nrspins * 100<<", ON SUNDAY: "<<nrspins * 85<<", SUNDAY + REBATE: "<<nrspins * 60<<"\n";
    cout<<"\n";
    }
    return nrspins;

}
int main()
{
    srand(time(0));
    cout<<"Enter gate parts: ";
    cin>>parts;

    int nr;
    int avg = 0;
    int lo = 2e9;
    int hi = 0;
    cout<<"How many times do you want to simulate?";
    cin>>nr;
    if(nr > 10000)
    {
        trigger = 0;
        cout<<"Not showing output due to being too slow!\n";
    }
    cout<<"\n";
    for(int i = 0; i < nr; ++i)
    {
        int res = simulate();
        avg += res;
        if(res > hi)
            hi = res;
        if(res < lo)
            lo = res;

        //cout<<"\n";
    }
    cout<<"===============================================\n";
    avg/= nr;
    cout<<"Average: "<<avg<<" spins - "<<(double)(avg/134.0)<<" hours collecting palladium \n";
    cout<<"Lowest number of spins: "<<lo<<" - "<<(double)(lo/134.0)<<" hours collecting palladium \n";
    cout<<"Highest number of spins: "<<hi<<" - "<<(double)(hi/134.0)<<" hours collecting palladium \n";
    cout<<"Average cost: "<<avg * 100<<"\nSunday: "<<avg * 85<<"\nSunday + rebate: "<<avg * 60<<"\nSunday + rebate + prem: "<<avg * 55<<"\n";

    return 0;
}
