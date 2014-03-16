#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#define MAX_BUFFER 1024

char* connectDB(char* header, int contentLen )
{
    char *       buffer = malloc( MAX_BUFFER * sizeof(char));
    int 			 len;
    char * 		 hostname = "cloudmine.me";
    int         port        = 443;
    int         s,rc;
    
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *hp;
    
    SSL_library_init ();
    SSL_load_error_strings ();
    SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    
    if ( (s = socket(AF_INET, SOCK_STREAM, 0 )) < 0 ){
        perror("socket failed");
        return 0;
    }
    
    memset((char*)&cli_addr, 0, sizeof(cli_addr));
    
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(0);
    cli_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    
    if ( bind(s, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0 ){
        perror("bind failed");
        return 0;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    hp = gethostbyname(hostname);
    
    if ( !hp ){
        perror("could not find host");
        return 0;
    }
    
    memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    
    SSL *conn = SSL_new(ssl_ctx);
    SSL_set_fd(conn, s);
    
    if ( connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr) )){
        perror("connect failed");
        return 0;
    }
    
    if ( SSL_connect(conn) <= 0 ){
        printf("connection error");
    }
    
    
/*    //TODO: implement a short write
    char* index = header;
    int lenSent = 0;
    if ((lenSent = (SSL_write( conn, header, strlen(header)+1 ))) != strlen(header)+1 ){
        printf("Did not send everything -- Retrying Now");
        while(lenSent != strlen(header)+1){
            index = index+lenSent;
            lenSent = lenSent + (SSL_write(conn, index,((strlen(header)+1)-lenSent)));
        }
    }
  */ 

	 if( SSL_write( conn, header, strlen(header)+1 ) != strlen(header)+1 ){
		 printf("This didnt print all");
	 }

    //TODO: implement a short read
    if ( SSL_read(conn, buffer, MAX_BUFFER-1) <= 0){
		 printf("This read failed");
	 }
    
    printf("BUFFER: %s \n LEN: %d \n", buffer, len+1);
    
    SSL_shutdown(conn);
    close(s);
    
    return buffer;
    
}
