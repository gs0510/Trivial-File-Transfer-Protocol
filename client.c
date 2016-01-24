#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netinet/in.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#define MAXLEN 512

int main(int argc,char* argv[])
{
  char buff[MAXLEN],file_name[MAXLEN];
  char file_buffer[MAXLEN];
  int sockfd,connfd,len,ret;
  struct sockaddr_in servaddr,cliaddr,servaddr1;
  
  // create socket in client side
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
 
  if(sockfd==-1)
  {
     printf(" socket not created in client\n");
     exit(0);
  }
  //initialising the socket
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(atoi(argv[1]));  // Port address

  //bzero(&servaddr1, sizeof(servaddr1));
  servaddr.sin_family = AF_INET;

while(1){
  printf("Type the file name\n");
  scanf("%s",file_name);
 
// send  msg to server
  sendto(sockfd, file_name, strlen(file_name), 0,(struct sockaddr *)&servaddr, sizeof(struct sockaddr));
  

  int count=0;
  len=sizeof(servaddr1);
  
  while(1)
  { 
    memset(file_buffer,0,sizeof file_buffer); 
    if ((ret = recvfrom(sockfd,file_buffer,MAXLEN,0,(struct sockaddr *)&servaddr1, &len))<0)
    {
      printf("error in recieving the datagram\n");
    }
    else{
      //if the file does not exist
      if(strcmp(file_buffer,"FILE DOES NOT  EXIST")==0){
        printf("File specified does not exist\n");
        break;
      }
      //sending an ACK to server
      count++;
      memset(buff, 0, sizeof buff);
      strcat(buff,"ACK:");
      strcat(buff,file_name);
      char scount[10];
      sprintf(scount,"%d",count);
      strcat(buff,":");
      strcat(buff,scount);
      strcat(buff,"\0");
      if(sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr *)&servaddr1, sizeof(struct sockaddr))<0){
        printf("Error in sending acknowledge\n");
      }
    }
  if(ret==0)break;
  }
}
  //close client side connection
  close(sockfd); 
  return(0);
}
