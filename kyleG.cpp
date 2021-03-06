// Author: Kyle Gregory
// Date-modified: 5/10/18
// Purpose: Modifies ship collisions, displays game menu, and displays high scores.

#include <iostream>
using namespace std;
#include "fonts.h"
#include "GL/glx.h"
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#define USERAGENT "HTMLGET 1.0"
#define PAGE "/"
#define PORT 80
#define HOST "coding.debuntu.org"


// Setup timers
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

// Prototypes
double testTime();

//-----------------------------------------------------------------------------
// Print name and time functions
//-----------------------------------------------------------------------------
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
// Send HighScore using HTTP
//-----------------------------------------------------------------------------
extern int playerScore;
char *highScores;
char *get_ip(char const *host);
int create_tcp_socket();
void program_usage();
void sendScore();
char *build_get_query(char const *host, char const *page);

void program_usage() {
    cout << stderr << "USAGE: htmlget host [page]\n\thost: the website "
	<< " hostname. ex: coding.debuntu.org\n\tpage: the page to retrieve. "
	<< " ex: index.html, default: /\n" << endl;
}

int create_tcp_socket() {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
	perror("ERROR: Unable to create TCP socket");
	exit(1);
    }
    return sock;
}

char *get_ip(char const *host) {
    struct hostent *hent;
    int iplen = 15;
    char *ip = (char *)malloc(iplen+1);
    memset(ip, 0, iplen+1);
    if ((hent = gethostbyname(host)) == NULL) {
	herror("ERROR: Unable to obtain IP host by name");
	exit(1);
    }
    if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen+1) == NULL) {
	perror("ERROR: Unable to resolve host with inet_ntop");
	exit(1);
    }
    return ip;
}

char *build_get_query(char const *host, char const *page) {
    char *query;
    char const *getpage = page;
    char const *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    if (getpage[0] == '/') {
	getpage = getpage + 1;
	cout << stderr << "Removing leading \"/\", converting " << page
	    << " to " << getpage << "\n" << endl;
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+
	    strlen(tpl)-5);
    sprintf(query, tpl, getpage, host, USERAGENT);
    return query;
}

void sendScore() {
    struct sockaddr_in *remote;
    int sock;
    int tmpres;
    char *ip;
    char *get;
    char buf[BUFSIZ+1];
    char const *host;
    char const *page;

    host = "sleipnir.cs.csubak.edu";
    page = "/~kgregory/3350/defendersOfSpace/scoreBoard.php";
    sock = create_tcp_socket();
    ip = get_ip(host);
    cout << stderr << "IP is " << ip << "\n" << endl;
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
    remote->sin_family = AF_INET;
    tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
    if (tmpres < 0) {
	perror("ERROR: Unable to set remote->sin_addr.s_addr\n");
	exit(1);
    } else if (tmpres == 0) {
	cout << stderr << ip << " is not a valid IP address\n" << endl;
	exit(1);
    }
    remote->sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0) {
	perror("ERROR: Unable to connect");
	exit(1);
    }
    get = build_get_query(host, page);
    cout << stderr << "Query is:\n<<START>>\n" << get << "<<END>>\n" << endl;

    //Send the query to the server
    int sent = 0;
    while (sent < (signed)strlen(get)) {
	tmpres = send(sock, get+sent, strlen(get)-sent, 0);
	if (tmpres == -1) {
	    perror("ERROR: Unable to send query\n");
	    exit(1);
	}
	sent += tmpres;
    }
    //Time to receive the page
    memset(buf, 0, sizeof(buf));
    int htmlstart = 0;
    char * htmlcontent;
    while ((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0) {
	if (htmlstart == 0) {
	    /* Under certain conditions this will not work.
	     * If the \r\n\r\n part is splitted into two messages
	     * it will fail to detect the beginning of HTML content
	     */
	    htmlcontent = strstr(buf, "\r\n\r\n");
	    if (htmlcontent != NULL) {
		htmlstart = 1;
		htmlcontent += 4;
	    }
	} else {
	    htmlcontent = buf;
	}
	if (htmlstart) {
	    cout << stdout << htmlcontent << endl;
	}
	memset(buf, 0, tmpres);
    }
    if (tmpres < 0) {
	perror("ERROR: Unable to receive data\n");
    }
    free(get);
    free(remote);
    free(ip);
    close(sock);
}

//-----------------------------------------------------------------------------
// SHIP modification
//-----------------------------------------------------------------------------

// Check for collision with window edges at x.res
int ship_xboundary (double x,int xres) { 
    if (x < 0.0) {
	return -1;
    }
    else if (x > xres) {
	return 1;
    } 
    return 0;
}

// Check for collision with window edges at y.res
int ship_yboundary (double y,int yres) {
    if (y < 0.0) {
	return -1;
    }
    else if (y > yres) {
	return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Game Menu
//-----------------------------------------------------------------------------
void showOptions(int xres, int yres) {
    glColor3ub(255,0,0);
    glPushMatrix();
    glTranslatef(xres/2.10, yres/4, 0.0);
    glTranslatef(-50.0, -50.0, 0.0); 
    glBegin(GL_QUADS);
    glVertex2i(-50, -65); 	//bottom left
    glVertex2i(-50, 130); 	//top left 
    glVertex2i(150, 130); 	//top right
    glVertex2i(150, -65); 	//bottom right
    glEnd();														

    Rect r;
    r.bot = 100;
    r.left = 50;
    r.center = 1;
    ggprint8b(&r, 16, 0x00000ff0, "GAME MENU");
    ggprint8b(&r, 16, 0x00ffff00, "C - Start game");
    ggprint8b(&r, 16, 0x00ffff00, "UP Arrow - Move up");
    ggprint8b(&r, 16, 0x00ffff00, "RIGHT Arrow - Move right");
    ggprint8b(&r, 16, 0x00ffff00, "BOTTOM Arrow - Move down");
    ggprint8b(&r, 16, 0x00ffff00, "LEFT Arrow - Move left");
    ggprint8b(&r, 16, 0x00ffff00, "W - Spread shot");
    ggprint8b(&r, 16, 0x00ffff00, "E - Charge beam");
    ggprint8b(&r, 16, 0x00ffff00, "I - Invincible");
    ggprint8b(&r, 16, 0x00ffff00, "ESC - Exit game");
    glPopMatrix();
}

void showWinner(int xres, int yres) {
    Rect r;
    r.bot = yres/2;
    r.left = xres/2;
    ggprint8b(&r, 16, 0x0041b9e1, "YOU WIN!");
}
