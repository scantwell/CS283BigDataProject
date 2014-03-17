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

#define MAX_BUFFER 1024 // max size of the buffer for the response of the socket

// takes the header which is the whole message to be sent to the server
// it includes content and header, content Len is the len of that message
// It connects using openssl because cloudmine.me ( the database we use )
// only allows https connection so normal sockets were used with openssl
// Returns the response
char* connectDB(char* header, int contentLen )
{
    char *       buffer = malloc( MAX_BUFFER * sizeof(char)); // used for the response from socket
    int 		 len; // length
    char * 		 hostname = "cloudmine.me"; // server address
    int          port = 443; // port to connect on
    int          s,rc; // socket names
    
    // struct for the server address and client
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *hp;
    
    // init the ssl lib and init a SSL_CTX certificate
    SSL_library_init ();
    SSL_load_error_strings ();
    SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    
    // Open socket
    if ( (s = socket(AF_INET, SOCK_STREAM, 0 )) < 0 ){
        perror("socket failed");
        return 0;
    }
    // clear memory of cli_addr struct
    memset((char*)&cli_addr, 0, sizeof(cli_addr));
    
    // defines the cli address that will be used
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(0);
    cli_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    // bind the cli port to the open socket s
    if ( bind(s, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0 ){
        perror("bind failed");
        return 0;
    }
    // init the server port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // converts port number
    
    hp = gethostbyname(hostname); // gets the IP address
    
    //make sure that we get an address response
    if ( !hp ){
        perror("could not find host");
        return 0;
    }
    
    // clears memory of server struct
    memcpy((void *)&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    
    // sets the connction with the certificate and sets the socket to the ssl connection
    SSL *conn = SSL_new(ssl_ctx);
    SSL_set_fd(conn, s);
    
    // connecting the socket to the server
    if ( connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr) )){
        perror("connect failed");
        return 0;
    }
    
    // Using the ssl conn to connect and talk
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
    }*/

    // Writes to the socket via SSL sending the header which is the full contents
	 if( SSL_write( conn, header, strlen(header)+1 ) != strlen(header)+1 ){
		 printf("This didnt print all");
	 }

    // read back from the socket and check for failure
    if ( SSL_read(conn, buffer, MAX_BUFFER-1) <= 0){
		 printf("This read failed");
	 }
    
    //printf("BUFFER: %s \n LEN: %d \n", buffer, len+1);
    
    // Close the socket and SSL connection
    SSL_shutdown(conn);
    close(s);
    
    return buffer;
    
}
