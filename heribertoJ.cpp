//Heriberto Jimenez
//DoS Project
//Last Modified 3/9/18


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include "GL/glx.h"
#include "log.h"
#include "fonts.h"

//---------------------------------------------------------------------------
//setup timers
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source); 

//---------------------------------------------------------------------------

//const Flt MINIMUM_ASTEROID_SIZE = 60.0;


double timeTester();

void showNameHeri(int x, int y) {
    static double t_time = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);
    Rect r;
    r.bot = y-200;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ffff00, "Heriberto");
    clock_gettime(CLOCK_REALTIME, &ftimeEnd);
    t_time += timeDiff(&ftimeStart, &ftimeEnd);
    ggprint8b(&r, 16, 0x00ffdd00, "function 1 time: %lf", t_time);
    ggprint8b(&r, 16, 0x00ffdd00, "function 2 time: %lf", timeTester());
}

double timeTester() {
    static double t_time = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);
    int i = 200000;
    while (i != 200000) {
	int j = 100, k = 200;
	j = j * 2000 * k * 1000;
	k = (k-500) & (j-800); 
	i++;
    }
    clock_gettime(CLOCK_REALTIME, &ftimeEnd);
    t_time += timeDiff(&ftimeStart, &ftimeEnd);
    return t_time;
}
/*
void shipMovement() {
	if (gl.keys[XK_Left]) {
         //apply thrust
         //convert ship angle to radians
         Flt rad = ((g.ship.angle+180.0) / 360.0f) * PI * 2.0;
         //convert angle to a vector
         if (g.ship.vel[0] > 0) {
             g.ship.vel[0] = 0;
         }
         Flt xdir = cos(rad);
         Flt ydir = sin(rad);
         g.ship.vel[0] += xdir*0.05f;
         g.ship.vel[1] += ydir*0.05f;
         Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                 g.ship.vel[1]*g.ship.vel[1]);
         if (speed > 5.0f) {
             speed = 5.0f;
             normalize2d(g.ship.vel);
             g.ship.vel[0] *= speed;
             g.ship.vel[1] *= speed;
         }
     }
     if (gl.keys[XK_Right]) {
         //apply thrust
         //convert ship angle to radians
         Flt rad = ((g.ship.angle+0.0) / 360.0f) * PI * 2.0;
         //convert angle to a vector
         if (g.ship.vel[0] < 0) {
             g.ship.vel[0] = 0;
         }
         Flt xdir = cos(rad);
         Flt ydir = sin(rad);
         g.ship.vel[0] += xdir*0.05f;
         g.ship.vel[1] += ydir*0.05f;
         Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                 g.ship.vel[1]*g.ship.vel[1]);
         if (speed > 5.0f) {
             speed = 5.0f;
             normalize2d(g.ship.vel);
             g.ship.vel[0] *= speed;
             g.ship.vel[1] *= speed;
         }
     }
     if (gl.keys[XK_Up]) {
         //apply thrust
         //convert ship angle to radians
         Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
         //convert angle to a vector
         if (g.ship.vel[1] < 0) {
             g.ship.vel[1] = 0;
         }
         Flt xdir = cos(rad);
         Flt ydir = sin(rad);
         g.ship.vel[0] += xdir*0.05f;
         g.ship.vel[1] += ydir*0.05f;
         Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                 g.ship.vel[1]*g.ship.vel[1]);
         if (speed > 5.0f) {
             speed = 5.0f;
             normalize2d(g.ship.vel);
             g.ship.vel[0] *= speed;
             g.ship.vel[1] *= speed;
         }
     }
     if (gl.keys[XK_Down]) {
         //apply thrust
         //convert ship angle to radians
         Flt rad = ((g.ship.angle+270.0) / 360.0f) * PI * 2.0;
         //convert angle to a vector
         if (g.ship.vel[1] > 0) {
             g.ship.vel[1] = 0;
         }
         Flt xdir = cos(rad);
         Flt ydir = sin(rad);
         g.ship.vel[0] += xdir*0.05f;
         g.ship.vel[1] += ydir*0.05f;
         Flt speed = sqrt(g.ship.vel[0]*g.ship.vel[0]+
                 g.ship.vel[1]*g.ship.vel[1]);
         if (speed > 5.0f) {
             speed = 5.0f;
             normalize2d(g.ship.vel);
             g.ship.vel[0] *= speed;
             g.ship.vel[1] *= speed;
         }
     }



}
*/


