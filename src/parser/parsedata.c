#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
//#include "cson_amalgamation_core.c"
//#include "cson.c"


void parseCsv(char* line, int cols)
{
    const char* toks;
    char** object;
    int objSize = 5;
    int i;
    int k = 0;
    cJSON *json, *root, *fmt;
    char * out;
    
    root=cJSON_CreateObject();
	cJSON_AddItemToObject(root, "name", cJSON_CreateString("DBname"));
	cJSON_AddItemToObject(root, "locations", json=cJSON_CreateArray());
    
   
    toks = strtok(line, "\\n,");
    
    for( i = 0; toks !=NULL ; toks = strtok(NULL, "\\n,"), i++){
        
    cJSON_AddItemToObject(json, "location", fmt=cJSON_CreateObject());
	cJSON_AddStringToObject(fmt,"type",		"rect");
	cJSON_AddNumberToObject(fmt,"width",		1920);
	cJSON_AddNumberToObject(fmt,"height",		1080);
	cJSON_AddFalseToObject (fmt,"interlace");
	cJSON_AddNumberToObject(fmt,"frame rate",	24);
    
        printf("This is tok: %s \n", cJSON_Print(root) );
        //if (!--num)
          //  return tok;
    }
    
    return;
}

void createJsonObject(char** items){
    
}

int main(int argc, char *argv[])
{
//    cson_value * objV = cson_value_new_object();
 //   cson_object * obj = cson_value_get_object(objV);
  //  cson_value * arV = cson_value_new_array();
   // cson_object_set( obj, "storeArray", arV );
	// Add some values to it:
    //cson_object_set( obj, "myInt", cson_value_new_integer(42) );
    //cson_object_set( obj, "myDouble", cson_value_new_double(42.24) );
    //cson_output_FILE( objV, stdout, NULL );
//    cson_array * ar = cson_value_get_array(arV);
    //cson_array_set( ar, i, cson_new_value_string("Hi, world!", 10) );

    FILE* stream = fopen(argv[1], "r");
    int i = 0;
    char line[1024];
    while (fgets(line, 1024, stream))
    {
	const char* name;
        char* tmp = strdup(line);
	char* tmp2 = strdup(line);
	char* tmp3 = strdup(line);
	parseCsv(tmp,5);
//	cson_array_set( ar, i, cson_new_value_string(name,strlen(name )));
	//printf("name would be %s\n", getfield(tmp, 5));
	//printf("addr would be %s\n", getfield(tmp2, 6));
	//printf("zip would be %s\n", getfield(tmp3, 7));
        free(tmp);
    }
  //  cson_output_FILE( objV, stdout, NULL);
    fclose(stream);

    
}
