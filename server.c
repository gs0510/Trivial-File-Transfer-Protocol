#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#define MAXLEN 512
#define RETRY 3
int main(int argc,char* argv[])
{ 
  char buff[MAXLEN];
  char file_buffer[MAXLEN];
  int sd,connfd,len;
  struct timeval tv;
  tv.tv_sec = 2;  // 2 Secs Timeout 
  tv.tv_usec = 0;  // Number of microseconds
  //creating a socket
  struct sockaddr_in servaddr,cliaddr;
  sd = socket(AF_INET, SOCK_DGRAM, 0); 
  if(sd==-1)
  {
    printf(" socket not created in server\n");
    exit(0);
  }
  
  //initialising the socket
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));
 
  //bind the socket to port specified in the command line argument
  if ( bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0 )
    printf("Not binded\n");
  
  len=sizeof(cliaddr);

  while(1)
  {
    //Create a child for every client
    memset(buff,0,sizeof buff);
    recvfrom(sd,buff,MAXLEN,0,(struct sockaddr *)&cliaddr, &len); //receive name of file from client
    if(fork()==0)
    {
      //create a socket in child
      struct sockaddr_in child_addr;
      int sdchild = socket(AF_INET,SOCK_DGRAM,0);
      bzero(&child_addr, sizeof(child_addr));
      child_addr.sin_family = AF_INET;
      child_addr.sin_addr.s_addr = INADDR_ANY;
      child_addr.sin_port = 0;
      //timeout for UDP socket
      setsockopt(sdchild, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
      
      bind(sdchild, (struct sockaddr *)&child_addr, sizeof(child_addr));
      
      //open the file specified by the client
      FILE *fp;
      fp=fopen(buff,"r");
      if(fp==NULL) //if the file does not exist send an error datagram
      {
        char error[] ="FILE DOES NOT  EXIST";
        if(sendto(sdchild,error,strlen(error),0,(struct sockaddr *)&cliaddr, len)<0)
        {
          printf("Error in sending datagram\n");
        }
        printf("file does not exist\n");
      }
      fseek(fp,0,SEEK_END);
      size_t file_size=ftell(fp);
      fseek(fp,0,SEEK_SET);
      
      int flag=0;
      int size;
      do
      {
        int retry =RETRY; //Number of times the server will send the datagram to child if acknowledge not received
        memset(file_buffer,0,sizeof file_buffer); 
        size = fread(file_buffer,1,sizeof file_buffer,fp); //Number of bytes read from the file
        if(size<=0) {flag=1;}
        while(retry--)
        {
          if(sendto(sdchild,file_buffer,strlen(file_buffer),0,(struct sockaddr *)&cliaddr, len)<0){
            printf("error in sending datagram\n");
          }
          if(recvfrom(sdchild,buff,MAXLEN,0,(struct sockaddr *)&cliaddr, &len)<0)
          {  
              printf("error in receiving datagram\n");
          } 
          else{
            printf("%s\n",buff );
            break;
          }
        }
        if(flag==1)break; //if no more data is present to read
        if(size < sizeof file_buffer){
          flag=1; //so that the next time a datagram of length 0 can be sent
        }
    }while(1); 
    close(sdchild);
  }  
    bzero(file_buffer,sizeof(file_buffer));
  }
  
  close(sd);
 
  return(0);
}
