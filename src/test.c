#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close */
#include <string.h>
#include <stdlib.h>

#define SERVER_PORT 80
#define MAX_MSG 100

int main (int argc, char *argv[]) {

  int sd, rc, i;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char* stuff;
  
  if(argc < 3) {
    printf("usage: %s <server> <data1> <data2> ... <dataN>\n",argv[0]);
    exit(1);
  }
  //printf("1");
  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("%s: unknown host '%s'\n",argv[0],argv[1]);
    exit(1);
  }

  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons(SERVER_PORT);
  
  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd<0) {
    perror("cannot open socket ");
    exit(1);
  }

  /* bind any port number */
  localAddr.sin_family = PF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);
  
  rc = bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr));
  if(rc<0) {
    printf("%s: cannot bind port TCP %u\n",argv[0],SERVER_PORT);
    perror("error ");
    exit(1);
  }
				
  /* connect to server */
  rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if(rc<0) {
    perror("cannot connect ");
    exit(1);
  }
    
  char * string = "GET /r/.json HTTP/1.1/Content-Type: application/text-html\n\r\n\r";
  
  //for(i=2;i<argc;i++) {
    
    rc = send(sd, string, strlen(string), 0);
    
    if(rc<0) {
      perror("cannot send data ");
      close(sd);
      exit(1);
    }
    rc = read(sd,stuff,strlen(stuff)); 
   // printf("%s: data%u sent (%s)\n",argv[0],i-1,argv[i]);
    printf("Response = %s\n", stuff);
   
 // }

return 0;
  
}
