// Author: Kyle Gregory
// Date-modified: 4/20/18
// Purpose: Modifies parts of the ship and displays scores through html file.

#include "fonts.h"
#include "GL/glx.h"
#include <ctime>
#include <fstream>
#include <iostream>
using namespace std;

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
// Scoreboard Creation

void createScores (int scores) {
    ofstream fout("scoreBoard.html");
    if (fout.is_open()) {
	fout << "<!DOCTYPE html>" << endl;
	fout << "<html>" << endl;
	fout << "<head>" << endl;
	fout << "<title>Defenders Of Space</title>" << endl;
	fout << "</head>" << endl;
	fout << "<body>" << endl;
	fout << "<h1>High Scores</h1>" << endl;
	fout << scores << endl;
	fout << "</body>" << endl;
	fout << "</html>" << endl;
    }
    fout.close();
}
//-----------------------------------------------------------------------------

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


//-----------------------------------------------------------------------------
// Game Menu

/*void displayMenu () {
	static float angle = 0.0; // Static to keep value
	glColor3ub(255,0,0);
	// Matrix will read bottom top when multiple translate/rotates together
	glPushMatrix();
	glTranslatef(x, y, 0.0);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-50.0, -50.0, 0.0); 
	angle = angle + 2.5; //rotation speed
	glBegin(GL_QUADS);
		glVertex2i(0, 0); 	//bottom left
		glVertex2i(0, 100); 	//top left 
		glVertex2i(100, 100); 	//top right
		glVertex2i(100, 0); 	//bottom right
	glEnd();														
	
	Rect r;
	r.bot = 50;
	r.left = 50;
	r.center = 1;
	ggprint8b(&r, 16, 0x00ffffff, "GAME MENU");
	ggprint8b
	glPopMatrix();
}*/
//-----------------------------------------------------------------------------
