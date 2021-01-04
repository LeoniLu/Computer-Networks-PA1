#include <sys/select.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
extern int h_errno;
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
//client
int main(int argc, char** argv){
	if(argc != 5){
		printf("incorrect input form.\n");
		exit(1);
	}
	char *clientname=argv[1];
	char *serverip=argv[2];
	char *serverport=argv[3];
	char *clientport=argv[4];
	char checki[20];
	strcpy(checki,serverip);
	int name = strcmp(clientname,"server");
	int ip = checkip(checki);
	int sp = atoi(serverport);
	int cp = atoi(clientport);
	int check = name && ip && (sp>1024) && (sp<65535) && (cp>1024) && (cp<65535);
	if(!check){
		fprintf(stderr,"incorrect input form.\n");
		exit(1);
	}else{
		printf(">>> ");
		fflush(stdout);
	}
//above: take input and check format
	int skt=socket(AF_INET,SOCK_DGRAM,0);
	if(skt<0){
		fprintf(stderr,"socket failed.");
		exit(1);
	}
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=inet_addr(serverip);
	serveraddr.sin_port=htons(sp);
//above: create socket for the server
	char hname[256];
	size_t len=sizeof(hname);
	gethostname(hname,len);
	struct hostent *content=gethostbyname(hname);
	char *ipaddr;
	ipaddr=inet_ntoa(*((struct in_addr*) content->h_addr_list[0])); 
//	in_addr_t myip=inet_addr(ipaddr);
//	printf("the ip address for myself is %s\n",ipaddr);
//above: get ip address for local host
	struct usrinfo userinfo;
	struct usrinfo *info=&userinfo;
	strcpy(info->name,clientname);
	strcpy(info->ip,ipaddr);
	info->port=cp;
	info->status=1;
//above: set up usrinfo for reg
	struct timeval wait_timeout;
	wait_timeout.tv_sec = 0;
	wait_timeout.tv_usec = 500000;
	setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, &wait_timeout, sizeof(wait_timeout));
//above: set up waiting time to 500msec for the socket
	struct timeval zero_time;
	zero_time.tv_sec=0;
	zero_time.tv_usec=0;
//above: set up a 0s timer zero_time
	char sendmsg[2048];
	memcpy(sendmsg+6,info,sizeof(*info));
	strcpy(sendmsg,"reg");
//above: set up the reg message to the server
	char receivemsg[2048];
	int number=0;
	int regstatus=1;
	char helpdereg[2048];
       	int hihi;
///////////test send many different message.

//////////

	sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
 	for(int i=0; i<5; i++){
        	if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL)>0)){
                	 if(!strcmp(receivemsg,"ack")){
                                  memcpy(&regstatus,receivemsg+6,4);
                                  break;
              		 }
                 }
		number++;
          }
          if(number==5){
                 printf("[Server not Responding.]\n>>> [Exiting]");
                 fflush(stdout);
                 exit(1);
          }else if(regstatus==1){
                 printf("[Client %s exists!!]\n>>> ",clientname);
                 fflush(stdout);
          }else{
                 printf("[Welcome you are registered.]\n>>> ");
                 fflush(stdout);
          }
//above: send the usrinfo to the server
	char input[256];
	struct sockaddr clientaddr;
	socklen_t lenaddr=sizeof(clientaddr);


	//entering the while loop for client mode-keep receiving and sending
	while(1){
               if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,&clientaddr,&lenaddr))>0){
                        char *fflag=receivemsg;
                        if(!strcmp(fflag,"send")){
                                char sender[20];
                                strcpy(sender,receivemsg+6);
                                char message[200];
                                strcpy(message,receivemsg+46);
                                printf("%s: %s\n>>> ",sender,message);
				fflush(stdout);
		     		strcpy(sendmsg,"ack");
				sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
			}else if(!strcmp(fflag,"info")){
				struct usrinfo thisinfo;
				int x;
				memcpy(&x,receivemsg+6,4);
				FILE *file=fopen("table","w");
				for(int i=0; i<x; i++){
					memcpy(&thisinfo,receivemsg+10+i*sizeof(thisinfo),sizeof(thisinfo));
					fwrite(&thisinfo,sizeof(thisinfo),1,file);
				}
				fclose(file);
				printf("[Client table updated.]\n>>> ");
				fflush(stdout);
			}else if(!strcmp(fflag,"hist")){
				int x;
				memcpy(&x,receivemsg+6,4);
				if(x!=0)
				printf("[You have messages]\n>>> ");
				fflush(stdout);
				for(int i=0; i<x; i++){
					struct mess here;
					struct mess *mes=&here;
					memcpy(mes,receivemsg+10+i*sizeof(here),sizeof(here));
					printf("%s: %s %s\n>>> ",mes->from, mes->time,mes->msg);	
					fflush(stdout);
				}
			}
		}

		FILE *fp = fopen("table","r");
		struct usrinfo r;
		struct List list;
		initList(&list);
		while(fread(&r,sizeof(r),1,fp)==1){
			struct table table1;
			struct table *piece = malloc(sizeof(table1));
			memcpy(piece,&r,sizeof(r));
			piece->next=list.head;
			list.head=piece;
		}
		fclose(fp);
//above: reading usrinfo's table from local file "table" into a linked list "list"
		fd_set rfds;
		FD_ZERO(&rfds);
          	FD_SET(0, &rfds);
		if(select(1, &rfds, NULL, NULL, &zero_time)){
			if(fgets(input,sizeof(input),stdin)){
			char null[2];
			null[0]='\n';
			null[1]=0;
			if(!strcmp(input,null)){
				printf(">>> ");
				fflush(stdout);
			}
			if(strcmp(input,null)){
				memset(sendmsg,0,sizeof(sendmsg));
				int lenm = strlen(input)-1;
                                        if(input[lenm]=='\n'){
                                                input[lenm]=0;
                                        }//truncate the \n at the end
				char *flag;
				flag=strtok(input," ");
				char *cname=strtok(NULL," ");
				if(cname==NULL){
					cname=null;
				}
				if(!strcmp(flag,"reg")&&!strcmp(clientname,cname)){
					struct usrinfo reginfo;
					strcpy(reginfo.name,cname);
					strcpy(reginfo.ip,ipaddr);
					reginfo.port=cp;
					reginfo.status=1;
					strcpy(sendmsg,"reg");
					memcpy(sendmsg+6,&reginfo,sizeof(reginfo));
					sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
					regstatus=1;
					number=0;
					for(int i=0; i<5; i++){
						if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL)>0)){
							if(!strcmp(receivemsg,"ack")){
								memcpy(&regstatus,receivemsg+6,4);
								break;
							}
						}
						number++;
					}
					if(number==5){
						printf(">>> [Server not Responding.]\n>>> [Exiting]");
						fflush(stdout);
						exit(1);
					}else if(regstatus==1){
						printf(">>> [Client %s exists!!]\n",cname);
						fflush(stdout);
					}else{
						printf(">>> [Welcome you are registered.]\n");
						fflush(stdout);
					}
				}else if(!strcmp(flag,"reg")&&strcmp(clientname,cname)){
					printf(">>> [You can only register into your account.]\n");
					fflush(stdout);
				}else if(!strcmp(flag,"dereg")&&(strcmp(cname,clientname))){
						printf(">>> [You can only deregister yourself.]\n");
						fflush(stdout);
				}else if(!strcmp(flag,"dereg")&&(!strcmp(cname,clientname))){
					strcpy(sendmsg,"dereg");
					struct usrinfo dereg;
					strcpy(dereg.name,cname);
					strcpy(dereg.ip,ipaddr);
					dereg.port=cp;
					dereg.status=0;
					memcpy(sendmsg+6,&dereg,sizeof(dereg));
					sendto(skt,sendmsg,sizeof(sendmsg),0,(const struct sockaddr *)&serveraddr,sizeof(serveraddr));
					number=0;
					for(int i=0; i<5; i++){
						if(recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL)>0){
							if(!strcmp(receivemsg,"ack"))
								break;
						}
						number++;
					}
					if(number==5){
						printf(">>> [Server not Responding.]\n>>> [Exiting]");
						fflush(stdout);
						exit(1);
					}else{
						printf(">>> [You are offline. Bye.]\n");
						fflush(stdout);
					}
				}else if(!strcmp(flag,"send")&&!strcmp(cname,clientname)){
						printf(">>> [message cannot send to yourself.]\n");
					        fflush(stdout);	
				}else if(!strcmp(flag,"send")&&strcmp(cname,clientname)){
					char *msg=strtok(NULL," ");
					if(msg==NULL){
						char nothing[]="";
						msg=nothing;
					}
	                                lenm = strlen(msg);
	                                msg[lenm]=' ';
					strcpy(sendmsg,"send");
					strcpy(sendmsg+6,clientname);
					strcpy(sendmsg+26,cname);
					strcpy(sendmsg+46,msg);
       		                        struct table *result=findtable(&list,cname,&strcmp);
					if(result!=NULL){
						if(result->status==1){
						struct sockaddr_in sendaddr;
                	                	memset(&sendaddr, 0, sizeof(sendaddr));
         	        	                sendaddr.sin_family=AF_INET;
                	        	        sendaddr.sin_addr.s_addr=inet_addr(result->ip);
	                        	        sendaddr.sin_port=htons(result->port);					
						sendto(skt,sendmsg,sizeof(sendmsg),0,(const struct sockaddr *)&sendaddr,sizeof(sendaddr));
						if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL))>0&&(!strcmp(receivemsg,"ack"))){
							printf(">>> [Message received by %s.]\n",cname);
							fflush(stdout);
						}else{
							printf(">>> [No ACK from %s, message sent to server.]\n",cname);
							fflush(stdout);
							strcpy(helpdereg,"dereg");
							strcpy(helpdereg+6,cname);
							sendto(skt,helpdereg,sizeof(helpdereg),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));                                      
							sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
							hihi=0;
							for(int i=0;i<5;i++){
								if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL)>0)&&(!strcmp(receivemsg,"ack"))){
									printf(">>> [Messages received by the server and saved]\n");
									fflush(stdout);
									break;
								}
								hihi++;
							}
							if(hihi==5){
								printf(">>> [[Server not Responding.]\n>>> [Exiting]");
                 						fflush(stdout);
               		  					exit(1);
							}
						}
						}else if(result->status==0){
							printf(">>> [The client is offline, message sent to server.]\n");
							sendto(skt,sendmsg,sizeof(sendmsg),0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
							hihi=0;
							for(int i=0; i<5; i++){
							if((recvfrom(skt,receivemsg,sizeof(receivemsg),0,NULL,NULL)>0)&&(!strcmp(receivemsg,"ack"))){     
                        	                                 printf(">>> [Messages received by the server and saved]\n");
                                	                         fflush(stdout);
                                        	        }
							hihi++;
							}
							if(hihi==5){
								printf(">>> [[Server not Responding.]\n>>> [Exiting]");
                                                                fflush(stdout);
                                                                exit(1);
                                                        }
						}
					}
				}
				printf(">>> ");
				fflush(stdout);
			}
			}///fgets the input
		}//detect input ready
		//removeAllNodes(&list);
	}//while loop end
}
