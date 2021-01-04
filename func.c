#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

int checkip(char *x){
	char *n;
	n=strtok(x,".");
	int count=0;
	while(n!=NULL){
		count++;
		int x = atoi(n);
		if(x<0||x>255){
			return 0;
		}
		n=strtok(NULL,".");
	}
	if(count!=4){
		return 0;
	}
	return 1;
}

struct table *findtable(struct List *list, char *dataSought, int (*compar)(const char *, const char *)){
	struct table *table1 = list->head;
	while(table1){
		if(strcmp(dataSought,table1->name)==0)
			return table1;
		table1 = table1->next;
	}
	return NULL;
}

struct messnode *findmessnode(struct ListM *list, char *dataSought, int (*compar)(const char *, const char *)){
        struct messnode *table = list->head;
        while(table){
                if(strcmp(dataSought,table->to)==0)
                        return table;
                table = table->next;
        }
        return NULL;
}
void popFront(struct List *list)
{
    struct table *oldHead = list->head;
    list->head = oldHead->next;
    free(oldHead);
}
void popFrontM(struct ListM *list)
{
	struct messnode *old = list->head;
	list->head = old->next;
	free(old);
}

struct messnode *popNode(struct ListM *list,struct messnode *node){
	struct messnode *first=NULL;
	struct messnode *second=NULL;
	first = list->head;
	if(first==NULL){
		return NULL;
	}else if(first==node){
		second=first->next;
		free(first);
		list->head=second;
		return list->head;
	}else{
		second = first->next;
		while(second!=NULL){
			if(second==node){
				struct messnode *delete=second;
				first->next=second->next;
				free(delete);
				return first;
			}
			second=second->next;
			first=first->next;
		}
	}
	return NULL;
}

void removeAllNodes(struct List *list)
{
    while (!isEmptyList(list))
        popFront(list);
}
void removeAllNodesM(struct ListM *list)
{
	while(!isEmptyListM(list))
		popFrontM(list);
}
