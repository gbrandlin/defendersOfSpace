// Author: Kyle Gregory
// Date-modified: 3/21/18

#include "fonts.h"
#include "GL/glx.h"
#include <ctime>

//-----------------------------------------------------------------------------
// Setup timers
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------

double testTime();

// Print name and time functions
void showNameKyle(int x, int y) {
    static double timer = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);
    Rect r;
    r.bot = y-800;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ffff00, "kyle");
    clock_gettime(CLOCK_REALTIME, &ftimeEnd);
    timer += timeDiff(&ftimeStart, &ftimeEnd);
    ggprint8b(&r, 16, 0x00ffdd00, "function 1 time: %lf", timer);
    ggprint8b(&r, 16, 0x00ffdd00, "function 2 time: %lf", testTime());
}

double testTime() {
    static double timer = 0.0;
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
    timer += timeDiff(&ftimeStart, &ftimeEnd);
    return timer;
}

//-----------------------------------------------------------------------------
// SHIP modification

// Check for collision with window edge x.res 
double ship_xboundary (double x) {
	if (x < 0.0) {
		x = 0.0;
	}
	else if (x > 1250.0) {
		x = 1250.0;
	}
	return x;
}

// Check for collision with window edge y.res
double ship_yboundary (double y) {
	if (y < 0.0) {
		y = 0.0;
	}
	else if (y > 900.0) {
		y = 900.0;
	}
	return y;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Bullet modification

// Check/delete for collision with window edge x.res
/*double bullet_boundary (double x, double y, int b) {
	if (x < 0.0) {
		b--;
	}
	else if (x > 1250.0) {
		b--;
	}
	else if (y < 0.0) {
		b--;
	}
	else if (y > 900.0) {
		b--;
	}
	return b;
}*/
//-----------------------------------------------------------------------------

