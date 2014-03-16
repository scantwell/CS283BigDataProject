#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "connection.c"

char *createDBentry(char *body){
	char* header = "PUT /v1/app/8b98552e5ad6425283215ea4d4339f7d/text HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\nContent-Type: application/json\r\nContent-Length: ";

int headLen = (strlen(header)+1);
int bodyLen = (strlen(body)+1);
int totalLen = headLen+bodyLen;
int tlen = strlen(body)+1;

strcat(body, "\r\n\r\n");
strcat(header, body);
totalLen = strlen(header);
    
char* fullReq = malloc((sizeof(char))*(totalLen + 1));

strcat(fullReq, header);
//strcat(fullReq, body);

char* objID = mainConnect(fullReq, totalLen);
return (char*)objID;
}

void deleteDBentry(char *key){

    char* headerp1 = "DELETE /v1/app/8b98552e5ad6425283215ea4d4339f7d/data?keys=";
    char* headerp2 = " HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\n\r\n";

    char reqBuf[256] = "";

    strcat(reqBuf, headerp1);
    strcat(reqBuf, key);
    strcat(reqBuf, headerp2);

    mainConnect(reqBuf, 256);

    return;
}

/*char *main(char cod, char* data){
	char* objID = "";
	if(cod == 'd'){
		deleteDBentry(data);
		return("0");
	}
	else{
		objID = createDBentry(data);
		return (char*)objID;
	}
}*/

int main ( ){
    
    char * db;
    
    db = createDBEntry("{ name: 23 }" );
    
    return 0;
}
