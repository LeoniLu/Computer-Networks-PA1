#ifndef __FUNC_H__
#define __FUNC_H__
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <arpa/inet.h>
int checkip(char *x);

struct mess {
	char from[20];
	char to[20];
	char time[10];
	char msg[200];
};

struct messnode {
	char from[20];
        char to[20];
        char time[10];
        char msg[200];
	struct messnode *next;
};
struct usrinfo {
	char name[20];
	char ip[16];
	int port;
	int status;
};

struct table {
        char name[20];
        char ip[16];
        int port;
        int status;
	struct table *next;
};

struct List {
    struct table *head;
};

static inline void initList(struct List *list)
{
    list->head = 0;
}

struct ListM{
	struct messnode *head;
};

static inline void initListM(struct ListM *list)
{
	list->head = 0;
}
struct table *findtable(struct List *list,char *dataSought,int (*compar)(const char *, const char *));
struct messnode *findmessnode(struct ListM *list,char *dataSought,int (*compar)(const char *, const char *));
struct messnode *popNode(struct ListM *list,struct messnode *node);

static inline int isEmptyList(struct List *list)
{
    return (list->head == 0);
}
static inline int isEmptyListM(struct ListM *list)
{
	return (list->head ==0);
}

void popFront(struct List *list);
void popFrontM(struct ListM *list);
void removeAllNodes(struct List *list);
void removeAllNodesM(struct ListM *list);
#endif
