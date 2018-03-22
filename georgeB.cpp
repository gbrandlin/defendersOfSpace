//George Brandlin 
//2/22/18
#include<iostream>
#include<stdio.h>
#include<cmath>
#include"fonts.h"
#include<GL/glx.h>
#include<ctime>
#include<cstdlib>


typedef float Flt;
typedef float Vec[3];
typedef Flt     Matrix[4][4];


class PowerUp {
    public:
	Vec pos;
	Vec vel;
	int nverts;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Powerup *prev;
	struct Powerup *next;

    public:
	PowerUp() {
	    prev = NULL;
	    next = NULL;
	}

};

class GlobalPow{
    public:
	PowerUp *ahead;

    public:
	GlobalPow(){
	    ahead = NULL;
	       
	}
} gp;



using namespace std;

extern double timeDiff(struct timespec *start,struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
double testFunc2();


void bulletsound(){

    cout << '\a';
    // cout << "where is the sound?";

}



void drawPowerUp() {

    //this function will generate at random a power pack allowing for more bullets to shoot out or making bullets bigger

    //Gives X ammount of power bullets to be used, will show a counter of how many are left to use

    //need to create class powerUp for the power up to move randomly across the screen


    float c1,c2,c3;
    c1= rand()% 200 +1;
    c2= rand()% 200 +1;
    c3= rand()% 200 +1;


    static float angle = 0.0;

    glColor3ub(c1,c2,c3);
    glPushMatrix();
    glTranslatef(100,200,0);
    glRotatef(angle,0.0f,0.0f,1.0f);
    glTranslatef(-25.0,-25.0,0.0);
    angle= angle +5.5;
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(0,50);
    glVertex2i(50,50);
    glVertex2i(50,0);
    glEnd();



    glPopMatrix();
}






void showNameGeorge(int x ,int y)
{

    static double t = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    clock_gettime(CLOCK_REALTIME,&ftimeStart);

    Rect r;

    r.bot= y- 200;
    r.left=10;
    r.center = 0;
    ggprint8b(&r,16,0x00ffff00,"GEORGE");
    clock_gettime(CLOCK_REALTIME,&ftimeEnd);
    t += timeDiff(&ftimeStart, &ftimeEnd);
    ggprint8b(&r,24,0x00ffff00,"function 1 time: %lf",t);
    ggprint8b(&r,24,0x00ffff00,"function 2 time: %lf", testFunc2());

}

double testFunc2()
{
    static double t = 0.0;
    struct timespec ftimeStart,ftimeEnd;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);

    //for(int i=0;i<100;i++){
    int a=200, b= 300;
    a = b * 5 / 2.2;
    b = a *7 / 13;
    //cout<<i;
    // }

    clock_gettime(CLOCK_REALTIME, &ftimeEnd);
    t += timeDiff(&ftimeStart, &ftimeEnd);
    return t;
}
