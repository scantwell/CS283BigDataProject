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

int main()
{
<<<<<<< HEAD

    char * hostname = "www.cloudmine.me";
    int         port        =  443;
=======
    char        buffer[MAX_BUFFER];
    int 			 len;
    char * 		 hostname = "cloudmine.me";
    int         port        = 443;
>>>>>>> c0ee28cf2d40fc73085191f1ad67bf063fb74520
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
<<<<<<< HEAD
    
    char * string = "GET /v1/app/8b98552e5ad6425283215ea4d4339f7d/text HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\n\r\n";
    
    if ( (send( s, string, strlen(string) + 1, 0 )) != strlen(string) ){
=======

	 if ( SSL_connect(conn) <= 0 ){
		 printf("connection error");
	 }

    char *header ="GET /v1/app/8b98552e5ad6425283215ea4d4339f7d/text HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\n\r\n"; 

	 //TODO: implement a short write
  	 if ( (SSL_write( conn, header, strlen(header)+1 )) != strlen(header)+1 ){
>>>>>>> c0ee28cf2d40fc73085191f1ad67bf063fb74520
        printf("Did not send everything");
    }

	 //TODO: implement a short read
    len = SSL_read(conn, buffer, MAX_BUFFER-1);
    
	 printf("BUFFER: %s \n LEN: %d \n", buffer, len+1);
    
	 SSL_shutdown(conn);
	 close(s);
    
    return 0;

}
