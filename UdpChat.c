#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "func.h"
#include <string.h>
int main(int argc, char **argv){
	char mode[3];
	if(argc<3){
		printf("incorrect input format.\n");
		exit(1);
	}
	strcpy(mode,argv[1]);
	if(!strcmp(mode,"-c")){
		if(argc==6)
		execl("./client","client",argv[2],argv[3],argv[4],argv[5],(char *)0);
	}else if(!strcmp("-s",mode)){
		if(argc==3)
		execl("./server","server",argv[2],(char *)0);
	}
	printf("incorrect input format.\n");
	return 0;
}
