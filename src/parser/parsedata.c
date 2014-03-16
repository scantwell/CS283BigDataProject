#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
//#include "cson_amalgamation_core.c"
//#include "cson.c"

#define WATERSHED 0
#define CORNERSTORE 1
#define PPRPARKS 2
#define PPRPLAYGROUNDS 3
#define PPRRECREATION 4

// defining a multi-arry for the index of the important columns in each db
char *colNames[5][5] = {
    
    {   "name", "description",   "longitude", "latitude", "url"},
    {   "name",     "address",     "zipcode",         "",    ""},
    {   "name",     "address",     "zipcode",         "",    ""},
    {"address", "description",        "name",         "",    ""},
    {   "name",     "address",     "zipcode",      "url",    ""}
};

int colIndex[5][5] = {
    
    { 1, 3, 19, 20, 21},
    { 4, 6, 7,   0,  0},
    { 1, 3, 4,   0,  0},
    { 1, 2, 3,   0,  0},
    { 1, 3, 4,   5,  0}
};

// Takes name of file, name of database, database macro
void parseCsv(char* fileName, char * dbName, int index )
{
    const char* toks;
//    char** object;
//   int objSize = 5;
    int i;
    int j;
    int k = 0;
    cJSON *json, *root, *fmt;
    char line[1024];;
    
    FILE* stream = fopen(fileName, "r");
    
    root=cJSON_CreateObject();
	cJSON_AddItemToObject(root, "name", cJSON_CreateString(dbName));
	cJSON_AddItemToObject(root, "locations", json=cJSON_CreateArray());

    for( i = 0; fgets(line, 1024, stream) != NULL; i++ )
    {
        
        if ( i != 0 ){
            
            cJSON_AddItemToObject(json, "location", fmt=cJSON_CreateObject());

            toks = strtok(line, "\n,");
            
            k = 0;
            
            for( j = 0; toks !=NULL ; toks = strtok(NULL, "\n,\r"), j++){
        
                //printf("This is the val: %d ----- %s \n", j, toks);

                if ( j == colIndex[index][k] && j != 0){
                    
                    //printf("This is the val: %d\n", j);//colNames[index][k]);
                    //printf("This is the val: %s         %s\n", colNames[index][k], toks);
                    cJSON_AddStringToObject(fmt, colNames[index][k], toks);
                    k++;
                }
            }
        /*
         cJSON_AddItemToObject(json, "location", fmt=cJSON_CreateObject());
         cJSON_AddStringToObject(fmt,"type",		"rect");
         cJSON_AddNumberToObject(fmt,"width",		1920);
         cJSON_AddNumberToObject(fmt,"height",		1080);
         cJSON_AddFalseToObject (fmt,"interlace");
         cJSON_AddNumberToObject(fmt,"frame rate",	24);
         */

        }
    }
    
    printf("This is tok: %s \n", cJSON_Print(root) );
    
    fclose(stream);
    
    return;
}

void createJsonObject(char** items){
    
}

int main(int argc, char *argv[])
{
    int i;
    
    char * dbNames[5] = { "Philadelphia_Green_Storm_Water_Infrastructure201302.csv",
                        "Philadelphia_Healthy_Corner_Stores201302.csv",
                        "Philadelphia_PPR_Parks_Points201302.csv",
                        "Philadelphia_PPR_Playgrounds201302.csv",
                        "Philadelphia_PPR_Recreation_Facilities201302.csv"
    };
    
    
    printf( " THIS IS ARGC %d", argc );
    for ( i = 0; i < 5; i++ ){
	
        parseCsv(dbNames[i], dbNames[i], 0);
    
    }
    return 0;
}
