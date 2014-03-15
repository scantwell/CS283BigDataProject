#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main()
{

    char * serveraddr = "https://cloudmine.me";
    int         iProtocolPort        = 80;
    char        szSmtpServerName[256] = "";
    char        szBuffer[4096]       = "";
    char        szMsgLine[1024]       = "";
    int         hServer;
    struct sockaddr_in SockAddr;

    hserver = socket(PF_INET, SOCK_STREAM, 0 );
    
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons((u_short)port);
    
    // Attempt to intialize WinSock (1.1 or later)
    if(WSAStartup(WINSOCK_VERSION, &WSData))
    {
        return 1;
    }

    // Lookup email server's IP address.
    lpHostEntry = gethostbyname(szSmtpServerName);
    if(!lpHostEntry)
    {
        printf("Cannot find SMTP mail server ");
        return 1;
    }

    // Create a TCP/IP socket, no specific protocol
    hServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(hServer == INVALID_SOCKET)
    {
        printf("Cannot open mail server socket");

        return 1;
    }

    // Use the HTTP default port if no other port is specified
    iProtocolPort = htons(80);

    // Setup a Socket Address structure
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port   = iProtocolPort;
    SockAddr.sin_addr   = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

    // Connect the Socket
    if(connect(hServer, (PSOCKADDR) &SockAddr, sizeof(SockAddr)))
    {
        printf("Error connecting to Server socket");
        printf("Last error = %d\n",WSAGetLastError());
        return 1;
     }

    int content_len = 0;    
    char *query=NULL;

    char * tpl = (char *)"DELETE /v1/app/8b98552e5ad6425283215ea4d4339f7d/data"
	"HTTP/1.1\r\nHost: %s\r\n"
        "X-CloudMine-ApiKey: 4cb5c78cdf4c11e09d3bfb656829e44b\r\n\r\n"

     //Get Time stamp
    char *currdate = currentDateTime();
    char *sign = NULL;
    char signString [32]= {0};
    //Generate sign string
    int ret = GenedateSignature(&sign,currdate);
    memcpy(signString,sign,28);

    query = (char *)malloc(512*(sizeof(char )));
    if(query == NULL)
    {

        LogMessage(LOG_FILE,"\nQuery == NULL \n");
        return FALSE;
    }
    memset(query,0,512);
    content_len=0;//strlen((char*)Status);
    sprintf(query, tpl);

    //Send data
    int r = send(hServer, query, strlen(query), 0);
    //Recv data
    recv(hServer, szBuffer, sizeof(szBuffer), 0);
    printf("\n%s",szBuffer);

    free(query);
    // Close server socket and prepare to exit.
    closesocket(hServer);

    WSACleanup();

    return 0;

}
