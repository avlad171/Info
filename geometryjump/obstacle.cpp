#include "obstacle.h"
#include <math.h>
#include <iostream>

obstacleUp::obstacleUp(double translate = 0.33, double elevation = -0.06)
{
    t1.x = -0.5 / 6. + translate;
    t1.y = elevation;

    t2.x = translate;
    t2.y = 1/6. + elevation;

    t3.x = 0.5 / 6. + translate;
    t3.y = elevation;

    p1.x = 0.5 / 6. + translate;
    p1.y = -0.3;

    p2.x = -0.5 / 6. + translate;
    p2.y = -0.3;

    type = 0;
}

obstacleUp::render()
{
        glPushMatrix();

        glLineWidth(3.0f);

        glBegin(GL_TRIANGLES);
        glColor3ub(0, 0, 0);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t2.x, t2.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t3.x, t3.y);
        glVertex2d(t2.x, t2.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t2.x, t2.y);
        glVertex2d(t1.x, t1.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t3.x, t3.y);
        glVertex2d(p1.x, p1.y);
        glVertex2d(p2.x, p2.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t3.x, t3.y);
        glVertex2d(p1.x, p1.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(p1.x, p1.y);
        glVertex2d(p2.x, p2.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(p2.x, p2.y);
        glVertex2d(t1.x, t1.y);
        glEnd();

        glPopMatrix();

}

obstacleUp::adjustHeight(double newHeight)
{
    if(newHeight < -0.3)
        newHeight = -0.3;
    if(newHeight > 0.4 - 1/6.)
        newHeight = 0.4 - 1/6.;

        t1.y = newHeight;
        t2.y = newHeight + 1/6.;
        t3.y = newHeight;
}
//@obstacledown

obstacleDown::obstacleDown(double translate = 0.33, double elevation = -0.06)
{
    p1.x = -0.5 / 6. + translate;
    p1.y = 1;

    p2.x = 0.5 / 6 + translate;
    p2.y = 1;

    t1.x = 0.5 / 6. + translate;
    t1.y = 1/6. + elevation;

    t2.x = translate;
    t2.y = elevation;

    t3.x = -0.5 / 6. + translate;
    t3.y = 1/6. + elevation;

    type = 1;
}


obstacleDown::render()
{
        glPushMatrix();

        glLineWidth(3.0f);

        glBegin(GL_TRIANGLES);
        glColor3ub(0, 0, 0);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t2.x, t2.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        //glPopMatrix();
        //return 0;

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t3.x, t3.y);
        glVertex2d(t2.x, t2.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t2.x, t2.y);
        glVertex2d(t1.x, t1.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0);
        glVertex2d(p1.x, p1.y);
        glVertex2d(p2.x, p2.y);
        glVertex2d(t1.x, t1.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(t1.x, t1.y);
        glVertex2d(p2.x, p2.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(p2.x, p2.y);
        glVertex2d(p1.x, p1.y);
        glEnd();

        glBegin(GL_LINES);
        glColor3ub(255, 255, 255);
        glVertex2d(p1.x, p1.y);
        glVertex2d(t3.x, t3.y);
        glEnd();

        glPopMatrix();

}

obstacleDown::adjustHeight(double newHeight)
{
    if(newHeight < -0.10)
        newHeight = -0.10;
    if(newHeight > 1 - 1/6.)
        newHeight = 1 - 1/6.;

        t1.y = newHeight + 1/6.;
        t2.y = newHeight;
        t3.y = newHeight + 1/6.;
}

void obstacle::advance()
{
    t1.x -= 0.03;
    t2.x -= 0.03;
    t3.x -= 0.03;
    p1.x -= 0.03;
    p2.x -= 0.03;
}

bool obstacle::isPassed()
{
    return t2.x < -1.5;
}

obstacle::~obstacle()
{

}

int check_collisions(float st_jos, obstacle** obstacles)
{
    //avem de verifica y0,-0.5 si y0, -0.375
    float st_sus = st_jos + 0.18;

    for(int i = 0; i < 3; ++i)
    {
        if(obstacles[i]==NULL)
            continue;
        if(obstacles[i]->type == 0)
        {
            if(obstacles[i]->t1.y > st_jos)
            {
                if(obstacles[i]->p2.x < -0.5 && obstacles[i]->p1.x > -0.5)
                    return 1;
                if(obstacles[i]->p2.x < -0.375 && obstacles[i]->p1.x > -0.375)
                    return 1;
            }

        }
        else
        {
            if(obstacles[i]->t1.y < st_sus)
            {
                //std::cout<<obstacles[i]->t1.y<<" "<<st_sus<<"\n";
                if(obstacles[i]->p1.x < -0.5 && obstacles[i]->p2.x > -0.5)
                    return 1;
                if(obstacles[i]->p1.x < -0.375 && obstacles[i]->p2.x > -0.375)
                    return 1;
            }
        }

    }
    return 0;
}
