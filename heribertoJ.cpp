//Heriberto Jimenez
//DoS Project
//Last Modified 3/9/18

#include "fonts.h"
#include "GL/glx.h"
#include <ctime>

//-----------------------------------------------------------------------------
//setup timers
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source); 

//----------------------------------------------------------------------------

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


