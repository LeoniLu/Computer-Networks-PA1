#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
extern int h_errno;
#include <unistd.h>
#include <string.h>
#include "func.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
//server
int main(int argc, char **argv){
	if(argc!=2){
		printf("incurrect input form.\n");
		exit(1);
	}
	
	char *serverport=argv[1];
	int sp = atoi(serverport);
	if(sp<1024||sp>65535){
		fprintf(stderr,"incorrect input form.\n");
		exit(1);
	}else{
		printf(">>> ");
		fflush(stdout);
	}
//above: take input from the user and check format
	char hname[256];
	size_t len=sizeof(hname);
	gethostname(hname,len);
	struct hostent *content=gethostbyname(hname);
	char *ipaddr;
	ipaddr=inet_ntoa(*((struct in_addr*) content->h_addr_list[0])); 
	printf("The ip address for the server is %s\n>>> ",ipaddr);
	fflush(stdout);
//above: acknowledge the user the client ip address (for client to connect)
	int skt = socket(AF_INET,SOCK_DGRAM,0);
	if(skt<0){
		fprintf(stderr,"socket failed.");
		exit(1);
	}
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=inet_addr(ipaddr);
	serveraddr.sin_port=htons(sp);
	 
//above: create socket and sockaddr_in for serveraddr

	struct timeval wait_timeout;
	wait_timeout.tv_sec = 0;
	wait_timeout.tv_usec = 500000;
	setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, &wait_timeout, sizeof(wait_timeout));
	if ( bind(skt, (struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0 ){ 
        	perror("bind failed."); 
        	exit(1); 
    	}
//above: set up the waiting time to 500msec for the socket
	char receivemsg[2048];
	char sendmsg[2048];
	struct mess m;
        struct ListM history;
        initListM(&history);
	struct usrinfo r;
        struct List list;
        initList(&list);
	struct sockaddr_in broadcast;

//above: reserved space for new variable decleration
	while(1){
		struct sockaddr clientaddr;
		socklen_t lenaddr=sizeof(clientaddr);
		if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,&clientaddr,&lenaddr))>0){
			char *flag=receivemsg;
			if(!strcmp(flag,"dereg")){
printf("dereg entered\n");
				char clientname[20];
				strcpy(clientname,receivemsg+6);
				struct table *deregtable = findtable(&list,clientname,&strcmp);
				if(deregtable != NULL){
				deregtable->status=0;
				strcpy(sendmsg,"ack");
				sendto(skt,sendmsg,sizeof(sendmsg),0,&clientaddr,lenaddr);
				
				//receive "dereg" update list of usrinfo, send "ack" to client
				memset(sendmsg,0,sizeof(sendmsg));
				struct table *infowrite = list.head;
				int usrnum=0;
				while(infowrite != NULL){
printf("*dereg list is %s %d %d\n",infowrite->name,infowrite->port,infowrite->status);
					memcpy(sendmsg+10+usrnum*sizeof(r),infowrite,sizeof(r));
					infowrite = infowrite->next;
					usrnum++;
				}
				//write new client table into "info"
				strcpy(sendmsg,"info");
				memcpy(sendmsg+6,&usrnum,4);
				infowrite=list.head;
printf("just before while\n");
				while(infowrite!=NULL){
printf("while entered\n");
					if(infowrite->status==1){
					memset(&broadcast,0,sizeof(broadcast));
					broadcast.sin_family=AF_INET;
					broadcast.sin_addr.s_addr=inet_addr(infowrite->ip);
					broadcast.sin_port=htons(infowrite->port);
printf("ip and port %s %s %d\n",infowrite->name,infowrite->ip,infowrite->port);
					sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&broadcast,sizeof(broadcast));
					}
					//broadcast table info to all active users
					infowrite=infowrite->next;
				}}
			}else if(!strcmp(flag,"reg")){
printf("reg entered\n");
				struct usrinfo reginfo;
				memcpy(&reginfo,receivemsg+6,sizeof(reginfo));
				struct table *infowrite;
				infowrite=list.head;
				while(infowrite!=NULL){
					if(!strcmp(infowrite->name,reginfo.name))
						break;
					infowrite=infowrite->next;
				}
				if(infowrite==NULL||(infowrite!=NULL&&infowrite->status==0)){
printf("register entered ack0 sent\n");
					struct table *findname = findtable(&list,reginfo.name,&strcmp);
					if(findname==NULL){
						struct table t;
						struct table *add = malloc(sizeof(t));
						int newport;
						newport=ntohs(((struct sockaddr_in *)&clientaddr)->sin_port);
						memcpy(add,&reginfo,sizeof(reginfo));
						memcpy(add+36,&newport,4);
						add->port=newport;
						add->next=list.head;
						list.head=add;
					}else{
printf("name exist figured out\n");
						reginfo.port=ntohs(((struct sockaddr_in *)&clientaddr)->sin_port);
						memcpy(findname,&reginfo,sizeof(r));
					}
struct table *hi;
hi=list.head;
while(hi != NULL){
printf("*reg list is %s %d %d\n",hi->name,hi->port,hi->status);
hi=hi->next;
}
					strcpy(sendmsg,"ack");
					int k=0;
					memcpy(sendmsg+6,&k,4);
			 		sendto(skt,sendmsg,sizeof(sendmsg),0, &clientaddr,lenaddr);
//send historical message
					memset(sendmsg,0,sizeof(sendmsg));
					int record=0;
					struct messnode *findmsg;
					findmsg = history.head;
					while(findmsg!=NULL){
						if(!strcmp(findmsg->to,reginfo.name)){
							memcpy(sendmsg+10+record*sizeof(m),findmsg,sizeof(m));
							record++;
printf("%d of message copied\n",record);

							findmsg=popNode(&history,findmsg);
printf("pop correct\n");
						}else{
printf("hold success\n");
if(findmsg!=NULL)
						findmsg=findmsg->next;
						}
printf("not finished next\n");
					}
					strcpy(sendmsg,"hist");
					memcpy(sendmsg+6,&record,4);
printf("# of message is %d\n",*(int *)(sendmsg+6));
					sendto(skt,sendmsg,sizeof(sendmsg),0,&clientaddr,lenaddr);
printf("hist message send to %s %d\n",inet_ntoa(*((struct in_addr*)&((struct sockaddr_in *)&clientaddr)->sin_addr.s_addr)),ntohs(((struct sockaddr_in *)&clientaddr)->sin_port));
				}else if(infowrite != NULL && infowrite->status ==1){
printf("user exist detected\n");
					strcpy(sendmsg,"ack");
					int k=1;
					memcpy(sendmsg+6,&k,4);
					sendto(skt,sendmsg,sizeof(sendmsg),0, &clientaddr,lenaddr);
				}
				memset(sendmsg,0,sizeof(sendmsg));
				int count=0;
				struct table *ui;
				ui=list.head;
				strcpy(sendmsg,"info");
				while(ui!=NULL){
					memcpy(sendmsg+10+count*sizeof(r),ui,sizeof(r));
					count++;
					ui = ui->next;
				}
				memcpy(sendmsg+6,&count,4);
printf("info message head is %s %d\n",sendmsg,*(int *)(sendmsg+6));
				//write updated table info to sendmsg;
				infowrite=list.head;
				while(infowrite!=NULL && infowrite->status==1){
					memset(&broadcast,0,sizeof(broadcast));
					broadcast.sin_family=AF_INET;
					broadcast.sin_addr.s_addr=inet_addr(infowrite->ip);
					broadcast.sin_port=htons(infowrite->port);
					sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&broadcast,sizeof(broadcast));
printf("broadcast message send to %d\n",ntohs(((struct sockaddr_in *)&broadcast)->sin_port));
					//broadcast table info to all active users
					infowrite=infowrite->next;
				}
			}else if(!strcmp(receivemsg,"send")){
				time_t now;
				struct tm *timenow;
				time(&now);
				timenow = localtime(&now);
				char stime[10];
				sprintf(stime,"%2d:%2d:%2d",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
				strcpy(sendmsg,"ack");
				sendto(skt,sendmsg,sizeof(sendmsg),0,&clientaddr,lenaddr);
				struct messnode *appendmsg=malloc(sizeof(m));
				strcpy(appendmsg->from,receivemsg+6);
				strcpy(appendmsg->to,receivemsg+26);
				strcpy(appendmsg->time,stime);
				strcpy(appendmsg->msg,receivemsg+46);
				appendmsg->next=NULL;
				struct messnode *messnode1=history.head;
				if(messnode1==NULL){
					history.head=appendmsg;
				}else{
					while(messnode1->next!=NULL){
						messnode1=messnode1->next;
					}
					messnode1->next=appendmsg;
				}
printf("message stored into his\n");
struct messnode *histsee=history.head;
while(histsee!=NULL){
	printf("*this is %s %s %s %s\n",histsee->from,histsee->to,histsee->time,histsee->msg);
	histsee=histsee->next;
}
			}
		}
	//	removeAllNodesM(&history);
	//	removeAllNodes(&list);


	}
}

