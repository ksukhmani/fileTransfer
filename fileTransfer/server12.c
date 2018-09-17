#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

void serviceClient(int);

int main(int argc, char *argv[]){  
  int sd, acpt, portNumber;
  struct sockaddr_in servAdd;     // server socket address
  if(argc != 2)
  {
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }
    //domain AF_INET uset for internet connection, SOCK_STREAM for reliable connection, 0 for low level protocol connection
  perror("\n----------------------------------------\n\n Waiting for a connection. Successful?\n\n----------------------------------------\n\n");
  sd = socket(AF_INET, SOCK_STREAM, 0); //socket created
   //values assigned in struct servAdd
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber); //portno is scanned and value is placed
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd)); //bind function is used to connect the socket  with servAdd values
  listen(sd,4); //listen waits for client connection with queue of 4

  while(1)  //infinite loop
  {
    acpt = accept(sd, NULL, NULL); //new socket is created for child process
    printf("Got a client\n\n\n");

    if(!fork())  //if it is child
    	serviceClient(acpt);
    close(acpt);
  }
}



void serviceClient(int sd){
  char putfile[255];
  char *input[2];
  char *fmsg1,*fmsg2,*fmsg3,*fmsg4;;
  FILE *fp;
  char file[255];
  int fd,i=0;
  char *token1;
  char *token2;
  while(1) //infinite loop for read/write purpose with client
  {
    if(read(sd, file, 255)<=0) //if nothing is read or no connection established
    {
      printf("\n\nBye Bye\n");
      close(sd); //close child socket
      exit(0);
    }
    token1 = strtok(file, " "); //strtok is used to tokenise
    while(token1!=NULL)
    {
      input[i]=token1;
      //printf("%s\n",token1);
      token1=strtok(NULL,"\n"); //place filename in token[1]
      i++;
    }

    if(((strcmp(input[0],"get"))!=0)&&((strcmp(input[0],"put"))!=0)) //if no get or put command passed
    {
      printf("\n\nWrong Input By User\n");
    }
    else if((strcmp(input[0],"put"))==0)
    {
      if(read(sd,putfile,255)<=0)
      {
        printf("\n\nDid not get reply from client\n");
      }
      if((strcmp(putfile,"$"))==0)
      {
        printf("\n\nDid not recieve file to upload\n");
      }
      else
      {
	int fd=open(input[1],O_WRONLY|O_CREAT|O_TRUNC,0700); //open file to write and download on server side 
	write(fd,putfile,strlen(putfile)+1);
	close(fd);
	fmsg2="File Uploaded\n";
	write(sd,fmsg2,strlen(fmsg2)+1); //pass fmsg2 to client
	printf("\n\n%s",fmsg2);
      }
    }
    else if((strcmp(input[0],"get"))== 0)
    {
	if((fp = fopen(input[1],"r"))==NULL) //open to download on client side
	{
          printf("\n\nNo such file exists\n");
	  fmsg3="$";
	  write(sd,fmsg3,strlen(fmsg3)+1);
	} 
	else
	{
	  int len;
	  char filec[255];
	  fseek(fp,0,SEEK_END);
	  len=ftell(fp); //ftell sets to curretn position
	  fseek(fp,0,SEEK_SET);
	  fread(filec,len,1,fp);
	  fclose(fp);
	  filec[len]='\0';
	  //printf("%s\n",filec);
	  write(sd,filec,strlen(filec)+1);
	  printf("file sent to client\n");
	}
      }
    }
}

