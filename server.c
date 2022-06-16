#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include<arpa/inet.h>
#define MAX 128
//data type used for saving students' information
typedef struct student
{
    char *id;
    char *email;
} student;
//array of all the students' information
typedef struct QueryTable
{
    int size;
    struct student table[1024];
} QueryTable;
//define the process of getting address from domain name by gethostbyname
int DNS(char* name, char * ip)
{
    struct hostent *host;
    struct in_addr **Addr;
    if((host=gethostbyname(name)) == NULL)
    {
        strcpy(ip,"URL address does not exist");
        return 1;
    }
    Addr = (struct in_addr **) host->h_addr_list;
    for(int i=0;Addr[i] != NULL ; i++)
    {
        strcpy(ip,inet_ntoa(*Addr[i]));
        return 0;
    }
    return 1;
}
//the process of searching all the QueryTable for some students' email
int Query(struct QueryTable Table,char* id , char *response)
{
    for(int i=0;i<Table.size;i++){
        if(strcmp(Table.table[i].id,id) == 0){
            strcpy(response,Table.table[i].email);
            return 1;
        }
    }
    strcpy(response,"No such student ID");
    return 0;
}
int main()
{
    //define or set some parameters used for socket
    struct sockaddr_in myaddr;
    struct sockaddr_in client_addr;
    char Query_table[1024][2];
    myaddr.sin_family = PF_INET;
    myaddr.sin_port = htons(1234);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // create socket
    int streamfd,status,sockfd = socket(PF_INET,SOCK_STREAM,0);
    int addr_size= sizeof(client_addr);
    // some values used for data exchange or satisfying requirement
    char str_buf[MAX] = { 0 };
    char domain_name[MAX] = {0};
    char info[128] = {0};
    char buff_id[128],buff_email[128];
    struct QueryTable Qtable;

    //read students' information and save it in QueryTable
    const char* filename = "query.txt";
    FILE* input_file = fopen(filename, "r");
    Qtable.size = 0;
    while (! feof (input_file) )
    {
        fscanf(input_file, "%s", buff_id);
        fscanf(input_file, "%s", buff_email);
        Qtable.table[Qtable.size].email = (char*)malloc(sizeof(buff_email));
        strcpy(Qtable.table[Qtable.size].email,buff_email);
        Qtable.table[Qtable.size].id = (char*)malloc(sizeof(buff_id));
        strcpy(Qtable.table[Qtable.size].id,buff_id);
        Qtable.size++;
    }

    //bind
    if (bind(sockfd, (struct sockaddr*)&myaddr,sizeof(myaddr))< 0) {
        perror("bind failure");
        exit(EXIT_FAILURE);
    }
    printf("bind success\n");
    

    while(1){
        //listen
        if (listen(sockfd, 10) < 0) 
        {
            perror("listen failure");
            exit(EXIT_FAILURE);
        }
        printf("listen success\n");

        //accept
        if ((streamfd = accept(sockfd,(struct sockaddr*)&client_addr,(socklen_t*)&addr_size))< 0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }    
        //the loop for providing service
        while(1){

            strcpy(str_buf,"What's your requirement? 1.DNS 2.QUERY 3.QUIT : ");
            //send the question "What's your requirement? 1.DNS 2.QUERY 3.QUIT : " to client
            write(streamfd,str_buf,strlen(str_buf));
            bzero(str_buf, MAX);
            bzero(info,127);
            //reveice requirement number from client
            read(streamfd,str_buf, MAX);
            //DNS service
            if(str_buf[0]=='1')
            {   
                strcpy(str_buf,"Input URL address : ");
                //send question
                write(streamfd,str_buf,strlen(str_buf));
                bzero(str_buf, MAX);
                //receive response from client
                read(streamfd,str_buf, MAX);
                //get address
                DNS(str_buf,info);
                strcpy(str_buf,info);
                //send address to client
                write(streamfd,str_buf,MAX);
            }
            else if(str_buf[0]=='2')
            {   
                strcpy(str_buf,"Input student ID : ");
                //send question
                write(streamfd,str_buf,strlen(str_buf));
                bzero(str_buf, MAX);
                //recive student id from client
                read(streamfd,str_buf, MAX);
                //get the email corresponding to the student id(if exists)
                Query(Qtable,str_buf,info);
                strcpy(str_buf,info);
                //send the email to client
                write(streamfd,str_buf,MAX);
            }
            else if(str_buf[0]=='3')
            {
                //if client leaves, jump out of this while loop and repeat the process of listen and accept.
                break;
            }
            else
            {
                //if client send some wrong number
                strcpy(str_buf,"Wrong number\n\n");
                write(streamfd,str_buf,strlen(str_buf));
                bzero(str_buf, MAX);
                break;
            }
        }
    }
    close(streamfd);
    return 0;
    
}

