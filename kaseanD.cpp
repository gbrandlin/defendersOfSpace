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
void circle(*a){      

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

