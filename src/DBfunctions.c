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

    char* objID;
    
   //strcat(body, "\r\n\r\n");
    
	char* header = "PUT /v1/app/8b98552e5ad6425283215ea4d4339f7d/text HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\nContent-Type: application/json\r\nContent-Length: ";
    
    int blen = strlen(body);
    
    char c[20];
    sprintf(c, "%d", blen);
    strcat(c, "\r\n\r\n");
    
    int tlen = strlen(header) + strlen(c) + blen + 9;
    
    char * content = (char*)malloc( tlen * sizeof(char) );
    char * index = content;
    
    strncpy(content, header, strlen(header) );
    index = (index + strlen(header));
    
    strncpy(index, c, strlen(c));
    index = (index + strlen(c));
    
    strncpy(index, body, blen);
    index = (index + blen);
    
    strncpy(index, "\r\n\r\n", 9);
    
    printf("header %s \n", content);

    objID = connectDB(content, tlen);
   // printf("header %s \n", objID);

    return objID;
}

void deleteDBentry(char *key){

    char* headerp1 = "DELETE /v1/app/8b98552e5ad6425283215ea4d4339f7d/data?keys=";
    char* headerp2 = " HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\n\r\n";

    char reqBuf[256] = "";

    strcat(reqBuf, headerp1);
    strcat(reqBuf, key);
    strcat(reqBuf, headerp2);

    connectDB(reqBuf, 256);

    return;
}
/*
char *main(char cod, char* data){
	char* objID = "";
	if(cod == 'd'){
		deleteDBentry(data);
		return("0");
	}
	else{
		objID = createDBentry(data);
		return (char*)objID;
	}
}

int main  (){
    
    char * db;
    
    //db = (char*)
		 deleteDBentry("23423fsdf" );
 	 printf("THIS IF BUFFER %s\n", db);   

	 return 0;
}*/
