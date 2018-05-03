// Author: Kyle Gregory
// Date-modified: 5/02/18
// Purpose: Modifies parts of the ship and displays scores through html file.

#include <iostream>
using namespace std;
#include "fonts.h"
#include "GL/glx.h"
#include <ctime>
#include <fstream>
/*#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#define USERAGENT "HTMLGET 1.0"
#define PAGE "/"
#define PORT 80
#define HOST "coding.debuntu.org"*/


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


//-----------------------------------------------------------------------------
// Send HighScore using HTTP
//-----------------------------------------------------------------------------
/*extern int playerScore;
char *highScores;
char *get_ip(char *host);
int create_tcp_socket();
void program_usage();
char *build_get_query(char *host, char const *page);

int main(int argc, char **argv) {
    struct sockaddr_in *remote;
    int sock;
    int tmpres;
    char *ip;
    char *get;
    char buf[BUFSIZ+1];
    char *host;
    char const *page;

    host = (char*)malloc(sizeof(char)*(20));
    page = (char*)malloc(sizeof(char)*(36));
    *host = "sleipnir.cs.csubak.edu";
    *page = "/~kgregory/3350/defendersOfSpace/scoreBoard.php";
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
            /*htmlcontent = strstr(buf, "\r\n\r\n");
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
    return 0;
}

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

char *get_ip(char *host) {
    struct hostent *hent;
    int iplen = 17;
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

char *build_get_query(char *host, char const *page) {
    char *buf = (char*)malloc(sizeof(char)*(20));
    sprintf(buf, "%d", playerScore);
    char *query;
    char *playerscore = (char*)malloc(sizeof(char)*(2));
    char *browser = (char*)malloc(sizeof(char)*(8));
    playerscore = buf;
    *page = (char*)malloc(sizeof(char)*(60));
    page="www.cs.csub.edu/~slopez2/cs335/sendToDatabase.php?param=";
    browser="firefox ";
    //char const *getpage = page;
    char *getpage=(char*)malloc(sizeof(char)*(60));
 
    strcpy(getpage, browser);
    strcat(getpage, page);
    strcat(getpage, playerscore);
    system(getpage);
    
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
}*/
//-----------------------------------------------------------------------------

            
//-----------------------------------------------------------------------------
// SHIP modification
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
