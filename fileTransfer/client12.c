#include<fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include<netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]){
  char uinp[255];
  char getfile[255];
  char file[255];
  int server, portNumber;
  struct sockaddr_in servAdd;     // server socket address
  
  if(argc != 3)
  {
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }
  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0) //if no connection is established with ip address
  {
    fprintf(stderr, " inet_pton() has failed\n");
    exit(2);
  }

  if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0) //create socket in client side
  {
    fprintf(stderr, "connect() has failed, exiting\n");
    exit(3);
  }

  while(1)
  {
    char fmsg1[255],fmsg2[255];
    FILE *fp;
    char *fmsg4;
    char *token1, *token2;
    int i=0;
    char *input[2];

    fprintf(stderr, "\nUse 'get' or 'put' for file transfer and 'quit' to exit\n\n");
    fgets(uinp, 254, stdin); //fgets function get message passed through keyboard
	
    if((strcmp(uinp,"quit\n"))==0) //if quit passed compare and close socket of client side
    {
      //printf("Quit entered by user\n");
	close(server);
	exit(0);
    }
    write(server, uinp, strlen(uinp)+1); //write message passed by client to server

    token1=strtok(uinp, " "); //tokenis the message
    while(token1!=NULL)
    {
      input[i]=token1;
      token1=strtok(NULL,"\n");
      i++;
    }
    if(((strcmp(input[0],"get"))!=0)&&((strcmp(input[0],"put"))!=0))  //comapre the commands
    {
      printf("\n\nIncorrect User Input\n");
    }
		
    else if((strcmp(input[0],"put"))==0)
    {
      if((fp=fopen(input[1],"r+"))==NULL)
      {
        printf("\n\nNo such file or directory available\n");
	fmsg4="$";
	write(server,fmsg4,strlen(fmsg4)+1);
	//printf("$ sent\n");
      }
	//put command to upload on clinet side
      else
      {
	int len;
	char filec[255];
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	fseek(fp,0,SEEK_SET);
	fread(filec,len,1,fp);
	fclose(fp);
	file[len]='\0';
	//printf("%s\n",filec);
	write(server,filec,strlen(filec)+1);
	//printf("file sent to server\n");
	if(read(server,fmsg2,255)<=0)
	{
          printf("\n\nDid not get confirmation\n");
	}
	else
	{
          printf("\n\n%s",fmsg2);
	}
			//}
      }
    }
    //get command to download file
    else if((strcmp(input[0], "get")) == 0)
    {
      //printf("inside get\n");
      if(read(server,getfile,255)<=0)
      {
        printf("\n\nDid not get reply from server\n");
	exit(0);
      }
      if(strcmp(getfile,"$")==0)
      {
	printf("\n\nNo such file available on server\n");
      }
      else
      {
        int fd=open(input[1],O_WRONLY|O_CREAT|O_TRUNC,0700);
	write(fd,getfile,strlen(getfile)+1);
	printf("\n\nFile Downloaded\n");
      }
    }
  }
}
