//Kasean Dunham
//Defenders of Space
//2/22/18

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
using namespace std;
#include <unistd.h>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include <ctime>

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
double testFunc();

using namespace std;
/*class Image {
  public:
//a texture ID
GLuint texId;
int width, height;
unsigned char *data;
~Image() {delete [] data; }
Image() {
system("convert asteroid.jpeg asteroid.ppm");
FILE *fpi = fopen("asteroid.ppm", "r");
if (fpi) {
char line[200];
fgets(line, 200, fpi);
fgets(line, 200, fpi);
while(line[0] == '#')
fgets(line, 200, fpi);
sscanf(line, "%i %i", &width, &height);
fgets(line, 200, fpi);
//get pixel data
int n = width * height * 3;
data = new unsigned char[n];
for(int i=0; i<n; i++)
data[i] = fgetc(fpi);
fclose(fpi);

}
unlink("asteroid.ppm");
}} img; */
/*
void circle(){      

    Asteroid *a = g.ahead;
    int i;
    int triangles = 100; // number of triangles


    float twoPi = 2.0f * 3.14159f;

    glBegin(GL_TRIANGLE_FAN);

    //glVertex2f(my_x,my_y); // origin

    for(i = 0; i <= triangles; i++) {

	glVertex2f((a->radius * cos(i *  twoPi / triangles)),
		(a->radius * sin(i * twoPi / triangles)));
    }
}*/

void showNamekasean(int a, int b)
{
    static double Time = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);
    Time += timeDiff(&ftimeStart, &ftimeEnd);
    Rect r;
    r.bot = b - 200;
    r.left = 18;
    r.center = 0;
    //ggprint8b(&r, 24, 0x00ffdd00, "Function time %lf", Time);
    ggprint8b(&r, 24, 0x00ffdd00, "Function time %lf", testFunc());
    clock_gettime(CLOCK_REALTIME, &ftimeEnd);

}

double testFunc()
{
    static double Time = 0.0;
    struct timespec ftimeStart, ftimeEnd;
    int i = 100000;
    clock_gettime(CLOCK_REALTIME, &ftimeStart);
    //for (int i = 0; i <= 100000; i++)
    while (i != 100000)
    {
	float x = 200.0;
	float y = 300.0;
	float z = 400.0;
	x = y * 3 / 3 + x / 3 - y / 3 * z * 100 / 2.2;
       	y = int(x) % 15 - int(y) % 14 - int(z) % 13;

	x += 0.2;
	y += 0.2;
	z += 0.2;
    }

    clock_gettime(CLOCK_REALTIME, &ftimeEnd);
    Time += timeDiff(&ftimeStart, &ftimeEnd);
    return Time;
}
