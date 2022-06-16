#include<unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 128
int main(){
    struct sockaddr_in server_addr;	
	int sockfd, status;	
	//set server address
	server_addr.sin_family = PF_INET;	
	server_addr.sin_port = htons(1234);	
	server_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");	
	//connect to the server	
	sockfd = socket (PF_INET, SOCK_STREAM, 0);
	connect (sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));	
	char str_buf[MAX]={0};
	while(1){
		bzero(str_buf, MAX);
		//read the question "What's your requirement? 1.DNS 2.QUERY 3.QUIT : " from server.
		read(sockfd, str_buf, MAX);
        printf("%s", str_buf);
		scanf("%s",str_buf);
		//response the server with requirement
        write(sockfd,str_buf,strlen(str_buf));
		//if choose DNS requirement
		if (str_buf[0] == '1'){
            bzero(str_buf,MAX);
			//read "Input URL address : " from server
			read(sockfd, str_buf, MAX);
        	printf("%s", str_buf);
			bzero(str_buf,MAX);
			scanf("%s",str_buf);
			//send URL address to server
        	write(sockfd,str_buf,strlen(str_buf));
			bzero(str_buf,MAX);
			//read the address from server
			read(sockfd, str_buf, MAX);
        	printf("address get from domain name : %s\n\n", str_buf);
        }
		else if (str_buf[0] == '2'){
            bzero(str_buf,MAX);
			//read "Input student ID : " from server
			read(sockfd, str_buf, MAX);
        	printf("%s", str_buf);
			bzero(str_buf,MAX);
			scanf("%s",str_buf);
			//send student id to server
        	write(sockfd,str_buf,strlen(str_buf));
			bzero(str_buf,MAX);
			//get the corresponding email from the server.
			read(sockfd, str_buf, MAX);
        	printf("Email get from server : %s\n\n", str_buf);
        }
		else if (str_buf[0] == '3'){
			//client leaves
			break;
		}
		else{
			//if input the wrong requirement
			bzero(str_buf,MAX);
			read(sockfd, str_buf, MAX);
        	printf("%s", str_buf);
			break;
		}
	}
	close(sockfd);	
	return 0;

}
    