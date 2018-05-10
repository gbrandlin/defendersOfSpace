//Title : DefendersOfSpace
//program: asteroids.cpp
//frame work by Gordon Grisel
// Authors: George, Kasean, Kyle, Heri
//date:    2014 - 2018
//mod spring 2015: added constructors
//mod spring 2018: X11 wrapper class
//
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
#include <stdio.h>

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%(a))
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
			     (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
//constants
const float TIMESLICE = 1.0f;
const float GRAVITY = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 1000;
const Flt MINIMUM_ASTEROID_SIZE = 60.0;

//-----------------------------------------------------------------------------
//Setup timers
const double OOBILLION = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-----------------------------------------------------------------------------

//initialize variables
int playerScore = 0;
int sentScores = 0;
int labFunctions = 0;
int showMenu = 1;
int shipLives = 3;
bool StartMenu = true;
bool GameOver = false;
bool ChargedBeam = false;
bool WavePowerUp = false;
bool ThrustBoost = false;
bool Invinsibility = false;

class Image {
    public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(const char *fname) {
	    if (fname[0] == '\0')
		return;
	    //printf("fname **%s**\n", fname);
	    int ppmFlag = 0;
	    char name[40];
	    strcpy(name, fname);
	    int slen = strlen(name);
	    char ppmname[80];
	    if (strncmp(name+(slen-4), ".ppm", 4) == 0)
		ppmFlag = 1;
	    if (ppmFlag) {
		strcpy(ppmname, name);
	    } else {
		name[slen-4] = '\0';
		//printf("name **%s**\n", name);
		sprintf(ppmname,"%s.ppm", name);
		//printf("ppmname **%s**\n", ppmname);
		char ts[100];
		//system("convert eball.jpg eball.ppm");
		sprintf(ts, "convert %s %s", fname, ppmname);
		system(ts);
	    }
	    //sprintf(ts, "%s", name);
	    FILE *fpi = fopen(ppmname, "r");
	    if (fpi) {
		char line[200];
		fgets(line, 200, fpi);
		fgets(line, 200, fpi);
		//skip comments and blank lines
		while (line[0] == '#' || strlen(line) < 2)
		    fgets(line, 200, fpi);
		sscanf(line, "%i %i", &width, &height);
		fgets(line, 200, fpi);
		//get pixel data
		int n = width * height * 3;
		data = new unsigned char[n];
		for (int i=0; i<n; i++)
		    data[i] = fgetc(fpi);
		fclose(fpi);
	    } else {
		printf("ERROR opening image: %s\n",ppmname);
		exit(0);
	    }
	    if (!ppmFlag)
		unlink(ppmname);
	}
};

Image img[5] = {
    "background.jpeg",
    "spaceship.gif",
    "Asteroid.png",
    "menu.png",
    "gameover.png",
};

class Global {
    public:
	int xres, yres;
	char keys[65536];
	Global() {
	    xres = 1250;
	    yres = 900;
	    memset(keys, 0, 65536);
	}
} gl;

class Spaceship{
    public:
	Vec pos;
	Vec vel;
} spaceship;

class Ship {
    public:
	Vec dir;
	Vec pos;
	Vec vel;
	float angle;
	float color[3];
    public:
	Ship() {
	    VecZero(dir);
	    pos[0] = (Flt)(gl.xres/2);
	    pos[1] = (Flt)(gl.yres/2);
	    pos[2] = 0.0f;
	    VecZero(vel);
	    angle = 0.0;
	    color[0] = color[1] = color[2] = 1.0;
	}
};

class Bullet {
    public:
	Vec pos;
	Vec vel;
	float radius;
	float color[3];
	struct timespec time;
    public:
	Bullet() { 
	    radius = 1.5;
	}
};

class Asteroid {
    public:
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
    public:
	Asteroid() {
	    prev = NULL;
	    next = NULL;
	}
};

class Game {
    public:
	Ship ship;
	Asteroid *ahead;
	//PowerUp *Ahead;
	Bullet *barr;
	int showSpaceship;
	int nasteroids;
	//int npowerups; //for powerup
	int nbullets;
	int ndestroyed;
	struct timespec bulletTimer;
	struct timespec mouseThrustTimer;
	bool mouseThrustOn;
	GLuint background;
	GLuint spaceship;
	GLuint meteroid;
	GLuint menu;
	GLuint gameover;
    public:
	Game() {
	    ahead = NULL;
	    barr = new Bullet[MAX_BULLETS+2];
	    showSpaceship = 0;
	    nasteroids = 0;
	    nbullets = 0;
	    mouseThrustOn = false;
	    //Kasean's code please dont touch!
	    //build 10 asteroids...
	    for (int j=0; j<30; j++) {
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = rnd()*10.0 + 40.0;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
		    a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
		    a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
		    angle += inc;
		}
		a->pos[0] = (Flt)(rand() % gl.xres);
		a->pos[1] = (Flt)(rand() % gl.yres);
		a->pos[2] = 0.0f;
		a->angle = 0.0;
		a->rotate = rnd() * 4.0 - 2.0;
		a->color[0] = 0.8;
		a->color[1] = 0.8;
		a->color[2] = 0.7;
		a->vel[0] = (Flt)(rnd()*2.0-1.0);
		a->vel[1] = (Flt)(rnd()*2.0-1.0);
		//std::cout << "asteroid" << std::endl;
		//add to front of linked list
		a->next = ahead;
		if (ahead != NULL)
		    ahead->prev = a;
		ahead = a;
		++nasteroids;
	    }

	    //X Windows variables
	    clock_gettime(CLOCK_REALTIME, &bulletTimer);
	}
	~Game() {
	    delete [] barr;
	}
} g;

//X Windows variables
class X11_wrapper {
    private:
	Display *dpy;
	Window win;
	GLXContext glc;
    public:
	X11_wrapper() {
	    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	    //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	    XSetWindowAttributes swa;
	    setup_screen_res(gl.xres, gl.yres);
	    dpy = XOpenDisplay(NULL);
	    if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server" << std::endl;
		exit(EXIT_FAILURE);
	    }
	    Window root = DefaultRootWindow(dpy);
	    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	    if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	    } 
	    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	    swa.colormap = cmap;
	    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
		StructureNotifyMask | SubstructureNotifyMask;
	    win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
		    vi->depth, InputOutput, vi->visual,
		    CWColormap | CWEventMask, &swa);
	    set_title();
	    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	    glXMakeCurrent(dpy, win, glc);
	    show_mouse_cursor(0);
	}
	~X11_wrapper() {
	    XDestroyWindow(dpy, win);
	    XCloseDisplay(dpy);
	}
	void set_title() {
	    //Set the window title bar.
	    XMapWindow(dpy, win);
	    XStoreName(dpy, win, "Defenders of Space");
	}
	void check_resize(XEvent *e) {
	    //The ConfigureNotify is sent by the
	    //server if the window is resized.
	    if (e->type != ConfigureNotify)
		return;
	    XConfigureEvent xce = e->xconfigure;
	    if (xce.width != gl.xres || xce.height != gl.yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	    }
	}
	void reshape_window(int width, int height) {
	    //window has been resized.
	    setup_screen_res(width, height);
	    glViewport(0, 0, (GLint)width, (GLint)height);
	    glMatrixMode(GL_PROJECTION); glLoadIdentity();
	    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
	    set_title();
	}
	void setup_screen_res(const int w, const int h) {
	    gl.xres = w;
	    gl.yres = h;
	}
	void swapBuffers() {
	    glXSwapBuffers(dpy, win);
	}
	bool getXPending() {
	    return XPending(dpy);
	}
	XEvent getXNextEvent() {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    return e;
	}
	void set_mouse_position(int x, int y) {
	    XWarpPointer(dpy, None, win, 0, 0, 0, 0, x, y);
	}
	void show_mouse_cursor(const int onoff) {
	    if (onoff) {
		//this removes our own blank cursor.
		XUndefineCursor(dpy, win);
		return;
	    }
	    //vars to make blank cursor
	    Pixmap blank;
	    XColor dummy;
	    char data[1] = {0};
	    Cursor cursor;
	    //make a blank cursor
	    blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
	    if (blank == None)
		std::cout << "error: out of memory." << std::endl;
	    cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
	    XFreePixmap(dpy, blank);
	    //this makes you the cursor. then set it using this function
	    XDefineCursor(dpy, win, cursor);
	    //after you do not need the cursor anymore use this function.
	    //it will undo the last change done by XDefineCursor
	    //(thus do only use ONCE XDefineCursor and then XUndefineCursor):
	}
} x11;

//function prototypes
void init_opengl();
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics();
void render();
void sendScores();
unsigned char *buildAlphaData(Image *img);
void init();


//==========================================================================
// M A I N
//==========================================================================
int main() {
    logOpen();
    init_opengl();
    srand(time(NULL));
    x11.set_mouse_position(100, 100);
    int done=0;
    extern int startMenuSound();
    startMenuSound();
    while (!done) {
	while (x11.getXPending()) {
	    XEvent e = x11.getXNextEvent();
	    x11.check_resize(&e);
	    check_mouse(&e);
	    done = check_keys(&e);
	}
	physics();
	render();
	x11.swapBuffers();
    }
    cleanup_fonts();
    logClose();
    return 0;
}

void sendScores() {
    if (sentScores == 1) {
	system("firefox www.cs.csub.edu/~kgregory/3350/scoreBoard.php");
	sentScores = 0;
    }
}

void init() {
    MakeVector(0.0,0.0,0.0, spaceship.pos);
    MakeVector(0.0,0.0,0.0, spaceship.vel);
}

void moveSpaceship() {
    //move spaceship...
    int addgrav = 1;
    //Update position
    spaceship.pos[0] += spaceship.vel[0];
    spaceship.pos[1] += spaceship.vel[1];
    //Check for collision with window edges
    if ((spaceship.pos[0] < -140.0 && spaceship.vel[0] < 0.0) ||
	    (spaceship.pos[0] >= (float)gl.xres+140.0 &&
	     spaceship.vel[0] > 0.0))
    {
	spaceship.vel[0] = -spaceship.vel[0];
	addgrav = 0;
    }
    if ((spaceship.pos[1] < 150.0 && spaceship.vel[1] < 0.0) ||
	    (spaceship.pos[1] >= (float)gl.yres && spaceship.vel[1] > 0.0)) {
	spaceship.vel[1] = -spaceship.vel[1];
	addgrav = 0;
    }
    //Gravity?
    if (addgrav)
	spaceship.vel[1] -= 0.75;
}

void init_opengl() {
    //OpenGL initialization
    glViewport(0, 0, gl.xres, gl.yres);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //This sets 2D mode (no perspective)
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    //----------------------------------------------------------------------
    //Backgorund texture
    int w = img[0].width;
    int h = img[0].height;
    glGenTextures(1, &g.background);
    glBindTexture(GL_TEXTURE_2D, g.background);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //This is where the texture is initialized in OpenGL (full sheet)
    unsigned char *background = buildAlphaData(&img[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, background);

    //---------------------------------------------------------------------
    //Spaceship texture
    int w1 = img[1].width;
    int h1 = img[1].height;
    glGenTextures(1, &g.spaceship);
    glBindTexture(GL_TEXTURE_2D, g.spaceship);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //This is where the texture is initialized in OpenGL (full sheet)
    unsigned char *spaceship = buildAlphaData(&img[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w1, h1, 0, GL_RGBA, GL_UNSIGNED_BYTE, spaceship);

    //----------------------------------------------------------------------
    //Asteroid texture
    int w2 = img[2].width;
    int h2 = img[2].height;
    glGenTextures(1, &g.meteroid);
    glBindTexture(GL_TEXTURE_2D, g.meteroid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //This is where the texture is initialized in OpenGL (full sheet)
    unsigned char *meteroid = buildAlphaData(&img[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w2, h2, 0, GL_RGBA, GL_UNSIGNED_BYTE, meteroid);

    //----------------------------------------------------------------------
    //Menu texture
    int w3 = img[3].width;
    int h3 = img[3].height;
    glGenTextures(1, &g.menu);
    glBindTexture(GL_TEXTURE_2D, g.menu);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //This is where the texture is initialized in OpenGL (full sheet)
    unsigned char *menu = buildAlphaData(&img[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w3, h3, 0, GL_RGBA, GL_UNSIGNED_BYTE, menu);

    //----------------------------------------------------------------------
    //Gameover texture
    int w4 = img[4].width;
    int h4 = img[4].height;
    glGenTextures(1, &g.gameover);
    glBindTexture(GL_TEXTURE_2D, g.gameover);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //This is where the texture is initialized in OpenGL (full sheet)
    unsigned char *gameover = buildAlphaData(&img[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w4, h4, 0, GL_RGBA, GL_UNSIGNED_BYTE, gameover);
}

//This function removes background from sprite sheet
unsigned char *buildAlphaData(Image *img) {
    //add 4th component to RGB stream...
    int i;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    unsigned char a, b, c;
    unsigned char t0 = *(data + 0);
    unsigned char t1 = *(data + 1);
    unsigned char t2 = *(data + 2);
    for (i = 0; i < img->width * img->height * 3; i += 3) {
	a = *(data + 0);
	b = *(data + 1);
	c = *(data + 2);
	*(ptr + 0) = a;
	*(ptr + 1) = b;
	*(ptr + 2) = c;
	*(ptr + 3) = 1;
	//if pixel is color of background set the 4th component to 0
	if (a == t0 && b == t1 && c == t2)
	    *(ptr + 3) = 0;
	//move to next pixel
	ptr += 4;
	data += 3;
    }
    return newdata;
}

void normalize2d(Vec v) {
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
	v[0] = 1.0;
	v[1] = 0.0;
	return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

void check_mouse(XEvent *e) {
    //Was a mouse button clicked?
    if (e->type != ButtonPress &&
	    e->type != ButtonRelease &&
	    e->type != MotionNotify)
	return;
    if (e->type == ButtonRelease)
	return;
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button is down
	    //a little time between each bullet
	    struct timespec bt;
	    clock_gettime(CLOCK_REALTIME, &bt);
	    double ts = timeDiff(&g.bulletTimer, &bt);
	    if (ts > 0.1) {
		timeCopy(&g.bulletTimer, &bt);
		//shoot a bullet...
		if (g.nbullets < MAX_BULLETS) {
		    Bullet *b = &g.barr[g.nbullets];
		    timeCopy(&b->time, &bt);
		    b->pos[0] = g.ship.pos[0];
		    b->pos[1] = g.ship.pos[1];
		    b->vel[0] = g.ship.vel[0];
		    b->vel[1] = g.ship.vel[1];
		    //convert ship angle to radians
		    Flt rad = ((g.ship.angle) / 360.0f) * PI * 2.0;
		    //convert angle to a vector
		    Flt xdir = cos(rad);
		    Flt ydir = sin(rad);
		    b->pos[0] += xdir*20.0f;
		    b->pos[1] += ydir*20.0f;
		    b->vel[0] += xdir*6.0f + rnd()*0.1;
		    b->vel[1] += ydir*6.0f + rnd()*0.1;
		    b->color[0] = 1.0f;
		    b->color[1] = 1.0f;
		    b->color[2] = 1.0f;
		    ++g.nbullets;
		}
	    }
	}
	if (e->xbutton.button==3) {
	    //Right button is down
	}
    }
}

int check_keys(XEvent *e) {
    //keyboard input?
    static int shift=0;
    if (e->type != KeyPress && e->type != KeyRelease)
	return 0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    if (e->type == KeyRelease) {
	gl.keys[key]=0;
	if (key == XK_Shift_L || key == XK_Shift_R)
	    shift=0;
	return 0;
    }
    gl.keys[key]=1;
    if (key == XK_Shift_L || key == XK_Shift_R) {
	shift=1;
	return 0;
    }
    (void)shift;
    switch (key) {
	case XK_Escape:
	    return 1;
	case XK_c:
	    showMenu = 0;
	    StartMenu = false;
	    break;
	case XK_k:
	    labFunctions = 0;
	    break;
	case XK_l:
	    labFunctions = 1;
	    break;
	case XK_m:
	    showMenu = 1;
	    break;
	case XK_s:
	    playerScore = g.ndestroyed;
	    sentScores = 1;
	    break;
	case XK_w:
	    WavePowerUp=!WavePowerUp;
	    break;
	case XK_e:
	    ChargedBeam=!ChargedBeam;
	    break;
	case XK_t:
	    ThrustBoost=!ThrustBoost;
	case XK_i:
	    Invinsibility=!Invinsibility;
    }
    return 0;

}

void deleteAsteroid(Game *g, Asteroid *node) {
    //Remove a node from doubly-linked list.
    if (node->prev == NULL) {
	if (node->next == NULL) {
	    //only 1 item in list.
	    g->ahead = NULL;
	} else {
	    //at beginning of list.
	    node->next->prev = NULL;
	    g->ahead = node->next;
	}
    } else {
	if (node->next == NULL) {
	    //at end of list.
	    node->prev->next = NULL;
	} else {
	    //in middle of list.
	    node->prev->next = node->next;
	    node->next->prev = node->prev;
	}
    }
    delete node;
    node = NULL;
    extern int explosion();
    explosion();
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a) {
    //build ta from a
    ta->nverts = 8;
    ta->radius = a->radius / 2.0;
    Flt r2 = ta->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ta->nverts;
    for (int i=0; i<ta->nverts; i++) {
	ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
	ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
	angle += inc;
    }
    ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
    ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
    ta->pos[2] = 0.0f;
    ta->angle = 0.0;
    ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
    ta->color[0] = 0.8;
    ta->color[1] = 0.8;
    ta->color[2] = 0.7;
    ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
    ta->vel[1] = a->vel[1] + (rnd()*2.0-1.0);
}

void physics() {
    g.ship.vel[0] *= .99;
    g.ship.vel[1] *= .99;
    if (g.showSpaceship) {
	moveSpaceship();
    }
    //c0 and c1 are the distances for the bullets
    Flt d0,d1,c0,c1,dist,dist1;
    //Update ship position
    g.ship.pos[0] += g.ship.vel[0];
    g.ship.pos[1] += g.ship.vel[1];
    //Check for ship collision with window edges
    extern int ship_xboundary (double,int);
    extern int ship_yboundary (double,int);
    if (int result = ship_xboundary (g.ship.pos[0],gl.xres)) {
	g.ship.vel[0] = -g.ship.vel[0];
	if (result < 0)
	    g.ship.pos[0] = 0.0;
	else
	    g.ship.pos[0] = (Flt)gl.xres;
    }
    if (int result = ship_yboundary (g.ship.pos[1],gl.yres)) {
	g.ship.vel[1] = -g.ship.vel[1];
	if (result < 0)
	    g.ship.pos[1] = 0.0;
	else
	    g.ship.pos[1] = (Flt)gl.yres;
    }

    //Update bullet positions
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    int i=0;
    while (i < g.nbullets) {
	Bullet *b = &g.barr[i];
	//How long has bullet been alive?
	double ts = timeDiff(&b->time, &bt);
	if (ts > 5.0) {
	    //time to delete the bullet.
	    memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
	    g.nbullets--;
	    continue;
	}
	//move the bullet
	b->pos[0] += b->vel[0];
	b->pos[1] += b->vel[1];
	//Check for collision with window edges
	if (b->pos[0] < 0.0) {
	    //delete the bullet...
	    memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
	    g.nbullets--;
	}
	else if (b->pos[0] > (float)gl.xres) {
	    memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
	    g.nbullets--;
	}
	else if (b->pos[1] < 0.0) {
	    memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
	    g.nbullets--;
	}
	else if (b->pos[1] > (float)gl.yres) {
	    memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
	    g.nbullets--;
	}
	i++;
    }

    //Update asteroid positions
    Asteroid *a = g.ahead;
    while (a) {
	a->pos[0] += a->vel[0];
	a->pos[1] += a->vel[1];
	//Check for collision with window edges
	if (a->pos[0] < -100.0) {
	    a->pos[0] += (float)gl.xres+200;
	}
	else if (a->pos[0] > (float)gl.xres+100) {
	    a->pos[0] -= (float)gl.xres+200;
	}
	else if (a->pos[1] < -100.0) {
	    a->pos[1] += (float)gl.yres+200;
	}
	else if (a->pos[1] > (float)gl.yres+100) {
	    a->pos[1] -= (float)gl.yres+200;
	}
	a->angle += a->rotate;
	a = a->next;
    }

    //Update bullet collisions
    a = g.ahead;
    while (a) {
	//collision for the ship
	int i = 0;
	d0 = g.ship.pos[0] - a->pos[0];
	d1 = g.ship.pos[1] - a->pos[1];
	dist = (d0*d0 + d1*d1);
	// If ship collides with asteroid
	//PowerUp on
	if (dist < (a->radius*a->radius) && Invinsibility) {
	    shipLives = 10;
	    if (shipLives > 0) {
		GameOver = false;
	    }
	}
	//PowerUp off
	else if (dist < (a->radius*a->radius) && !Invinsibility) {
	    shipLives = shipLives - 1 ;
	    if (shipLives == 0) {
		GameOver = true;
	    }
	}
	while (i < g.nbullets) {
	    Bullet *b = &g.barr[i];
	    c0 = b->pos[0] - a->pos[0];
	    c1 = b->pos[1] - a->pos[1];
	    dist1 = (c0*c0 + c1*c1);
	    if (dist1 < (a->radius*a->radius)) {
		//cout << "asteroid hit." << endl;
		if (a->radius > MINIMUM_ASTEROID_SIZE) {
		    //break it into pieces.
		    Asteroid *ta = a;
		    buildAsteroidFragment(ta, a);
		    int r = rand()%10+5;
		    for (int k=0; k<r; k++) {
			//get the next asteroid position in the array
			Asteroid *ta = new Asteroid;
			buildAsteroidFragment(ta, a);
			//add to front of asteroid linked list
			ta->next = g.ahead;
			if (g.ahead != NULL)
			    g.ahead->prev = ta;
			g.ahead = ta;
			g.nasteroids++;
		    }
		} else {
		    a->color[0] = 1.0;
		    a->color[1] = 0.1;
		    a->color[2] = 0.1;
		    //delete the asteroid and bullet
		    Asteroid *savea = a->next;
		    deleteAsteroid(&g, a);
		    a = savea;
		    g.nasteroids--;
		}
		//delete the bullet...
		memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
		g.nbullets--;
		g.ndestroyed++;
		if (a == NULL)
		    break;
	    }
	    i++;
	}
	if (a == NULL)
	    break;
	a = a->next;
    }

    //check keys pressed now
    //if (gl.keys[XK_Left]) {
    if (gl.keys[XK_Left] && ThrustBoost) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+180.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[0] > 0) {
	    g.ship.vel[0] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.20f;
	g.ship.vel[1] += ydir*0.20f;
    } else if (gl.keys[XK_Left]) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+180.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[0] > 0) {
	    g.ship.vel[0] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.10f;
	g.ship.vel[1] += ydir*0.10f;
    }
    //if (gl.keys[XK_Right]) {
    if (gl.keys[XK_Right] && ThrustBoost) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+0.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[0] < 0) {
	    g.ship.vel[0] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.20f;
	g.ship.vel[1] += ydir*0.20f;
    } else if (gl.keys[XK_Right]) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+0.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[0] < 0) {
	    g.ship.vel[0] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.10f;
	g.ship.vel[1] += ydir*0.10f;

    }
    //if (gl.keys[XK_Up]) {
    if (gl.keys[XK_Up] && ThrustBoost) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[1] < 0) {
	    g.ship.vel[1] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.20f;
	g.ship.vel[1] += ydir*0.20f;
    } else if (gl.keys[XK_Up]) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+90.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[1] < 0) {
	    g.ship.vel[1] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.10f;
	g.ship.vel[1] += ydir*0.10f;
    }
    //if (gl.keys[XK_Down]) {
    if (gl.keys[XK_Down] && ThrustBoost) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+270.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[1] > 0) {
	    g.ship.vel[1] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.20f;
	g.ship.vel[1] += ydir*0.20f;
    } else if (gl.keys[XK_Down]) {
	//apply thrust
	//convert ship angle to radians
	Flt rad = ((g.ship.angle+270.0) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	if (g.ship.vel[1] > 0) {
	    g.ship.vel[1] = 0;
	}
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	g.ship.vel[0] += xdir*0.10f;
	g.ship.vel[1] += ydir*0.10f;
    }
    if (gl.keys[XK_space] && !ChargedBeam) {
	if (!WavePowerUp) {
	    //a little time between each bullet
	    struct timespec bt;
	    clock_gettime(CLOCK_REALTIME, &bt);
	    double ts = timeDiff(&g.bulletTimer, &bt);
	    if (ts > 0.1) {
		timeCopy(&g.bulletTimer, &bt);
		if (g.nbullets < MAX_BULLETS) {
		    //shoot a bullet...
		    Bullet *b = &g.barr[g.nbullets];
		    timeCopy(&b->time, &bt);
		    b->pos[0] = g.ship.pos[0] + 20.0;
		    b->pos[1] = g.ship.pos[1] ;
		    b->vel[0] = 6.0f;
		    b->vel[1] = g.ship.vel[1];
		    g.nbullets++;
		    extern int bulletsound();
		    bulletsound();
		}
	    }
	} else {
	    //a little time between each bullet
	    static int bCount = 0;
	    struct timespec bt;
	    clock_gettime(CLOCK_REALTIME, &bt);
	    double ts = timeDiff(&g.bulletTimer, &bt);
	    if (ts > 0.1) {
		timeCopy(&g.bulletTimer, &bt);
		if (g.nbullets < MAX_BULLETS) {
		    //shoot a bullet...
		    Bullet *b1= &g.barr[g.nbullets];
		    Bullet *b2= &g.barr[g.nbullets+1];
		    Bullet *b3= &g.barr[g.nbullets+2];
		    timeCopy(&b1->time, &bt);
		    timeCopy(&b2->time, &bt);
		    timeCopy(&b3->time, &bt);
		    b1->pos[0] = b2->pos[0] =b3->pos[0] = g.ship.pos[0];
		    b1->pos[1] = b2->pos[1] =b3->pos[1] = g.ship.pos[1];
		    b1->vel[0] = b2->vel[0] =b3->vel[0] = 6.0f;
		    b1->vel[1] = b2->vel[1]= b3->vel[1] = g.ship.vel[1];
		    b2->vel[1]+=2.0;
		    b3->vel[1]-=2.0;
		    g.nbullets+=3;
		    extern int bulletsound();
		    bulletsound();
		    bCount++;
		    if (bCount > 1) {
			WavePowerUp=false;
			bCount = 0;
		    }
		}

	    }
	}
    }
    if (ChargedBeam) {
	static bool made = false;
	if (gl.keys[XK_space] == 1) {
	    if (!made) {
		Bullet *b = &g.barr[g.nbullets];
		timeCopy(&b->time, &bt);
		b->radius = 1.0;
		b->pos[0] = g.ship.pos[0] + 20.0+b->radius;
		b->pos[1] = g.ship.pos[1] - 3.0;
		b->vel[0] = 0.0;
		b->vel[1] = 0.0;
		g.nbullets++;
		made = true;
	    } else {
		Bullet *b = &g.barr[g.nbullets-1];
		b->radius += .07;
		if (b->radius>10.0) {
		    b->radius = 8.0;
		}
		b->pos[0] = g.ship.pos[0] + 20.0+b->radius;
		b->pos[1] = g.ship.pos[1] - 3.0;
	    }
	} else if (gl.keys[XK_space] == 0) {
	    Bullet *b = &g.barr[g.nbullets-1];
	    b->vel[0] = 6.0f;
	    b->vel[1] = g.ship.vel[1];
	    made = false;
	}		 
    }

    //if (g.mouseThrustOn) {
    if (g.mouseThrustOn) {
	struct timespec mtt;
	clock_gettime(CLOCK_REALTIME, &mtt);
	double tdif = timeDiff(&mtt, &g.mouseThrustTimer);
	if (tdif < -0.3) {
	    g.mouseThrustOn = false;
	}
    }
}

//Kasean
void renderGameOver() {
    glClear(GL_COLOR_BUFFER_BIT);

    //render gameover screen//
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.gameover);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    float TextWidth = (float)1.0;
    float TextHeight = (float)1.0 * -.8;
    float textureX = 0;
    float textureY = 0;
    float centerX = gl.xres / 2;
    float centerY = (gl.yres / 2);
    float width = img[3].width;
    float height = img[3].height;

    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY + TextHeight);
    glVertex2i(centerX - width, centerY - height);
    glTexCoord2f(textureX, textureY);
    glVertex2i(centerX - width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY);
    glVertex2i(centerX + width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY + TextHeight);
    glVertex2i(centerX + width, centerY - height);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

//Kasean
void renderStartMenu() {
    glClear(GL_COLOR_BUFFER_BIT);

    //render starting menu//
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.menu);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    float TextWidth = (float)1.0;
    float TextHeight = (float)1.0 * -.8;
    float textureX = 0;
    float textureY = 0;
    float centerX = gl.xres / 2;
    float centerY = (gl.yres / 2);
    float width = img[3].width;
    float height = img[3].height;

    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY + TextHeight);
    glVertex2i(centerX - width, centerY - height);
    glTexCoord2f(textureX, textureY);
    glVertex2i(centerX - width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY);
    glVertex2i(centerX + width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY + TextHeight);
    glVertex2i(centerX + width, centerY - height);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void PowerUp() {
    float c1;
    float c2;
    float c3;

    c1 = rand()% 200 + 1;
    c2 = rand()% 200 + 1;
    c3 = rand()% 200 + 1;

    for (int i = 0; i <= 100; i++) {
	float dirX;
	float dirY;
	float move1 = 5;
	float move2 = 5;

	dirX = 1000 + sin(move1);
	dirY = 500 + cos(move2);

	static float angle = 0.0;

	glColor3ub(c1,c2,c3);
	glPushMatrix();
	glTranslatef(dirX,dirY,0.0);
	glRotatef(angle,0.0f,0.0f,1.0f);
	glTranslatef(-25.0,-25.0,0.0);
	angle = angle + 5.5;
	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(0,50);
	glVertex2i(50,50);
	glVertex2i(50,0);
	glEnd();
	glPopMatrix();
    }
}

// Kasean
void Background() {
    glClear(GL_COLOR_BUFFER_BIT);

    //render background//
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.background);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    float TextWidth = (float)1.0;
    float TextHeight = (float)1.0 * -.8;
    float textureX = 0;
    float textureY = 0;
    float centerX = gl.xres / 2;
    float centerY = (gl.yres / 2);
    float width = img[0].width;
    float height = img[0].height;

    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY + TextHeight);
    glVertex2i(centerX - width, centerY - height);
    glTexCoord2f(textureX, textureY);
    glVertex2i(centerX - width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY);
    glVertex2i(centerX + width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY + TextHeight);
    glVertex2i(centerX + width, centerY - height);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void showScore() {
    Rect r;
    r.bot = gl.yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, 0x00ff0000, "Defenders of Space");
    ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g.nbullets);
    ggprint8b(&r, 16, 0x00ffff00, "n asteroids: %i", g.nasteroids);
    ggprint8b(&r, 16, 0x00ffff00, "n asteroids destroyed: %i", g.ndestroyed);    
}

// Kasean
void drawShip() {
    float TextWidth = (float)1.0;
    float TextHeight = (float)1.0 * -.8;
    float textureX = 0;
    float textureY = 0;
    float centerX = gl.xres / 2;
    float centerY = (gl.yres / 2);
    float width;
    float height;

    Background();

    //Draw the ship
    glColor3fv(g.ship.color);
    glPushMatrix();
    glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
    glRotatef(g.ship.angle-90.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, g.spaceship);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    TextWidth = (float)1.0;
    TextHeight = (float)1.0;
    textureX = 0;
    textureY = 0;
    centerX = (gl.xres / 500); //moves image to the dot
    centerY = (gl.yres / 256); //moves image to the dot
    width = img[1].width / 8;
    height = img[1].height / 8;

    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY + TextHeight);
    glVertex2i(centerX - width, centerY - height) ;
    glTexCoord2f(textureX, textureY);
    glVertex2i(centerX - width, centerY + height) ;
    glTexCoord2f(textureX + TextWidth, textureY);
    glVertex2i(centerX + width, centerY + height);
    glTexCoord2f(textureX + TextWidth, textureY + TextHeight);
    glVertex2i(centerX + width, centerY - height);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    if (ThrustBoost) {
	//draw thrust
	Flt rad = ((g.ship.angle) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	Flt xs,ys,xe,ye,r;
	glBegin(GL_LINES);
	for (int i = 0; i < 16; i++) {
	    xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
	    ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
	    r = rnd()*40.0+40.0;
	    xe = -xdir * r + rnd() * 18.0 - 9.0;
	    ye = -ydir * r + rnd() * 18.0 - 9.0;
	    glColor3f(0, 255, 255);
	    glVertex2f(g.ship.pos[0]+xs,g.ship.pos[1]+ys);
	    glVertex2f(g.ship.pos[0]+xe,g.ship.pos[1]+ye);
	}
	glEnd();
    }
    if (gl.keys[XK_Right] && !ThrustBoost) {
	//draw thrust
	Flt rad = ((g.ship.angle) / 360.0f) * PI * 2.0;
	//convert angle to a vector
	Flt xdir = cos(rad);
	Flt ydir = sin(rad);
	Flt xs,ys,xe,ye,r;
	glBegin(GL_LINES);
	for (int i = 0; i < 16; i++) {
	    xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
	    ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
	    r = rnd()*40.0+40.0;
	    xe = -xdir * r + rnd() * 18.0 - 9.0;
	    ye = -ydir * r + rnd() * 18.0 - 9.0;
	    glColor3f(rnd()*.3+.7, 255, 0);
	    glVertex2f(g.ship.pos[0]+xs,g.ship.pos[1]+ys);
	    glVertex2f(g.ship.pos[0]+xe,g.ship.pos[1]+ye);
	}
	glEnd();
    }
}

// CHANGED
void showBullet() {
    //Draw the bullets
    Bullet *b = &g.barr[0];
    for (int i=0; i<g.nbullets; i++) {
	//Log("draw bullet...\n");
	Flt radColor = 1.0-(b->radius-1.0)/9.0;
	glColor3f(1.0, 1.0*radColor, 1.0*radColor);
	int triangles = 10; // number of triangles
	float twoPi = 2.0f * 3.14159f;

	glBegin(GL_TRIANGLE_FAN);
	for (int j = 0; j <= triangles; j++) { 
	    glVertex2f((b->pos[0]+b->radius * cos(j * twoPi / triangles)), 
		    (b->pos[1]+b->radius * sin(j * twoPi / triangles)));
	}
	glEnd();
	++b;
    }
}

void drawAsteroid() {
    float TextWidth = (float)1.0;
    float TextHeight = (float)1.0 * -.8;
    float textureX;
    float textureY;
    float centerX = gl.xres / 2;
    float centerY = (gl.yres / 2);
    float width = img[0].width;
    float height = img[0].height;

    //Draw the asteroids
    Asteroid *a = g.ahead;
    while (a) {
	glColor3fv(a->color);
	glPushMatrix();
	glTranslatef(a->pos[0], a->pos[1], a->pos[2]); //moves the shapes and attaches shpe to the moving dot
	glBindTexture(GL_TEXTURE_2D, g.meteroid);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255, 255, 255, 255);

	TextWidth = (float)1.0;
	TextHeight = (float)1.0;
	textureX = 0;
	textureY = 0;
	centerX = (gl.xres / 500); //moves image to the dot
	centerY = (gl.yres / 256); //moves image to the dot
	width = img[2].width / 8;
	height = img[2].height / 8;

	glBegin(GL_QUADS);
	glTexCoord2f(textureX, textureY + TextHeight);
	glVertex2i(centerX - width, centerY - height) ;
	glTexCoord2f(textureX, textureY);
	glVertex2i(centerX - width, centerY + height) ;
	glTexCoord2f(textureX + TextWidth, textureY);
	glVertex2i(centerX + width, centerY + height);
	glTexCoord2f(textureX + TextWidth, textureY + TextHeight);
	glVertex2i(centerX + width, centerY - height);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255,255,255,255);
	glBegin(GL_TRIANGLES);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(a->pos[0], a->pos[1]);
	glEnd();
	a = a->next;
    }
}

void render() {

    Background();
    
    extern void showNameKyle(int, int);
    extern void showNameHeri(int, int);
    if (labFunctions == 1) {
	showNameKyle(gl.xres, gl.yres);
	//	showNamekasean(100, 600);
	showNameHeri(100, 500);
    }

    drawShip();
    showBullet();
    drawAsteroid();
    showScore();

    extern void showOptions(int, int);
    extern void showWinner(int, int);
    if (g.nasteroids == 0) {
	showWinner(gl.xres, gl.yres);
    } else if(GameOver == true) { 
	renderGameOver();
    }

    if (StartMenu) {
	renderStartMenu();
	if (showMenu == 1) {
	    showOptions(gl.xres, gl.yres);
	}
    }
}
