#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <GL/gl.h>

struct punct
{
    double x, y;
};

class obstacle
{
public:
    //obstacle();
    ~obstacle();

    int type;

    double height;
    punct t1, t2, t3, p1, p2;   //3 puncte pentru triunghi, 2 pentru colturile de jos de la patrat
                                //ordinea e p1 - p2 - t1 - t2 - t3 - p1 - p2 - ... in sensul acelor de ceas

    bool isPassed();
    void advance();
    virtual render() =0;
    virtual adjustHeight(double) =0;

};

class obstacleUp : public obstacle
{
public:
    obstacleUp(double, double);
    render();
    adjustHeight(double);

};

class obstacleDown : public obstacle
{
public:
    obstacleDown(double, double);
    render();
    adjustHeight(double);
};
/*class obstacleUp
{
    public:
        obstacleUp(double, double);
        virtual ~obstacleUp();

        double height;

        double TL, BL, TR, BR;

        punct t1, t2, t3;
        render();
        advance();
        bool isPassed();
        adjustHeight(double);
};

class obstacleDown
{
public:
    obstacleDown(double, double);
    virtual ~obstacleDown();
    double height;
    double TL, BL, TR, BR;
    punct t1, t2, t3;

    render();
    advance();
    bool isPassed();
    adjustHeight(double);
};*/

int check_collisions(float st_jos, obstacle ** obstacles);

#endif // OBSTACLE_H
