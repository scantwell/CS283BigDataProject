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


// Creates the POST message to be sent to the host server
// takes body which is the Json string
char *createDBentry(char *body){

    char* objID;
    
    // sets the header
	char* header = "PUT /v1/app/8b98552e5ad6425283215ea4d4339f7d/text HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\nContent-Type: application/json\r\nContent-Length: ";
    
    // find the length of the body arg
    int blen = strlen(body);
    
    //casts a number to a string and appends \r\n\r\n to it
    char c[20];
    sprintf(c, "%d", blen);
    strcat(c, "\r\n\r\n");
    
    // find length of total content message
    int tlen = strlen(header) + strlen(c) + blen + 9;
    
    // allocate memory for the content
    char * content = (char*)malloc( tlen * sizeof(char) );
    char * index = content; // assigns a pointer to content
    
    // copy header to context and increment the index pointer length of header
    strncpy(content, header, strlen(header) );
    index = (index + strlen(header));
    
    // copy c and move the index pointer down length of c
    strncpy(index, c, strlen(c));
    index = (index + strlen(c));
    
    // copy body and move index pointer down length of body
    strncpy(index, body, blen);
    index = (index + blen);
    
    // append the last line
    strncpy(index, "\r\n\r\n", 9);
    
   // printf("header %s \n", content);
    
    // send the object to the server
    objID = connectDB(content, tlen);
   // printf("header %s \n", objID);

    return objID;
}

// Creates a DELETE message to send to the server to delete a record
// takes a key of the object in the database
void deleteDBentry(char *key){
    
    // inits the headers with the keys
    char* headerp1 = "DELETE /v1/app/8b98552e5ad6425283215ea4d4339f7d/data?keys=";
    char* headerp2 = " HTTP/1.1\r\nHost: www.cloudmine.me\r\nX-CloudMine-ApiKey: 4a1bbce6b8864246a52262fe920dad52\r\n\r\n";

    // init a buffer to hold the content
    char reqBuf[256] = "";
    
    // copy all data into reqBuf
    strcat(reqBuf, headerp1);
    strcat(reqBuf, key);
    strcat(reqBuf, headerp2);

    // send content message to the server to be deleted
    connectDB(reqBuf, 256);

    return;
}