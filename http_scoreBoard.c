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

extern int playerScore;
char *highScores;
char *get_ip(char *);
int create_tcp_socket();
void usage();
void sendScore();
char *build_get_query(char *, char *);

char *get_ip(char *host) {
    struct hostent *hent;
    int iplen = 17;
    char *ip = (char *)malloc(iplen+1);
    memset(ip, 0, iplen+1);
    if ((hent = gethostbyname("sleipnir.cs.csub.edu")) == NULL) {
        herror("ERROR: Unable to obtian IP\n");
        exit(1);
    }
    if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL) {
        perror("ERROR: Unable to resolve host\n");
        exit(1);
    }
    return ip;
}

int create_tcp_socket() {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("ERROR: Unable to create TCP socket\n");
        exit(1);
    }
    return sock;
}

char *build_get_query(char *host, char *page) {
    char *buf = (char*)malloc(sizeof(char)*(20));
    sprintf(buf,"%d", playerScore);
    char *query;
    char *playerscore = (char*)malloc(sizeof(char)*(2));
    char *browser = (char*)malloc(sizeof(char)*(8));
    playerscore = buf;
    *page = (char*)malloc(sizeof(char)*(42));
    page = "www.cs.csub.edu/~kgregory5/3350/scoreBoard.php?param=";
    browser = "firefox ";
    char *getpage = (char*)malloc(sizeof(char)*(60));
    strcpy(getpage, browser);
    strcat(getpage, page);
    strcat(getpage, playerscore);
    system(getpage);
    char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
    if (getpage[0] == '/') {
        getpage = getpage + 1;
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
    sprintf(query, tpl, getpage, host, USERAGENT);
    return query;
}

void usage() {
    fprintf(stderr, "USAGE: htmlget host [page]\n\
            \thost: the website hostname. ex: coding.debuntu.org\n\
            \tpage: the page to retrieve. ex: index.html, default: /\n");
}

void send() {
    struct sockaddr_in *remote;
    int sock;
    int tmpres;
    char *ip;
    char *get;
    char buf[BUFSIZ+1];
    char *host;
    char *page;
    host = (char*)malloc(sizeof(char)*(20));
    page = (char*)malloc(sizeof(char)*(36));
    *host = "sleipnir.cs.csub.edu";
    *page = "/~kgregory5/3350/scoreBoard.php";
    sock = create_tcp_socket();
    ip = get_ip(host);
    fprintf(stderr, "IP is %s\n", ip);
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
    remote->sin_family = AF_INET;
    tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
    if ( tmpres < 0) {
        perror("ERROR: Unable to set remote->sin_addr.s_addr\n");
        exit(1);
    } else if (tmpres == 0) {
        fprintf(stderr, "%s is not a valid IP address\n", ip);
        exit(1);
    }
    remote->sin_port = htons(PORT);
    if (connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0) {
        perror("ERROR: Unable to connect.\n");
        exit(1);
    }
    get = build_get_query(host, page);
    fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
    //Send query to the server
    int sent = 0;
    while (sent < strlen(get)) {
        tmpres = send(sock, get+sent, strlen(get)-sent, 0);
        if (tmpres == -1) {
            perror("ERROR: Unable to send query\n");
            exit(1);
        }
        sent += tmpres;
    }
    //Receive the page
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
            fprintf(stdout, htmlcontent);
        }
        memset(buf, 0, tmpres);
    }
    if (tmpres < 0) {
        perror("ERROR: Unable to receive data.\n");
    }
    free(get);
    free(remote);
    free(ip);
    close(sock);
}
