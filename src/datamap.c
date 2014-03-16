//////////////////////////////////////////////////
//
//	Team:		Michael Brisbin, Stephen Cantwell, Dana McGrath
// Course:	CS283-601
// Task:		Final Project -- user interface
// Due:		Monday, March 17, 2014, 0800
//
/////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "cJSON.h"
#include "DBfunctions.c"

//Global
#define MAX_DATABASE 5 // Max databases

// Defining the index for the parallel arrays colNames, colIndex
#define WATERSHED 0
#define CORNERSTORE 1
#define PPRPARKS 2
#define PPRPLAYGROUNDS 3
#define PPRRECREATION 4

// Filenames of the databases
char * dbFileNames[5] = {
    "./db/Philadelphia_Green_Storm_Water_Infrastructure201302.csv",
    "./db/Philadelphia_Healthy_Corner_Stores201302.csv",
    "./db/Philadelphia_PPR_Parks_Points201302.csv",
    "./db/Philadelphia_PPR_Playgrounds201302.csv",
    "./db/Philadelphia_PPR_Recreation_Facilities201302.csv"
};

// defining a multi-arry for the index of the important columns in each db
char *colNames[5][5] = {
    
    {   "name", "description",   "longitude", "latitude", "url"},
    {   "name",     "address",     "zipcode",         "",    ""},
    {   "name",     "address",     "zipcode",         "",    ""},
    {"address", "description",        "name",         "",    ""},
    {   "name",     "address",     "zipcode",      "url",    ""}
};
// defines multi-array for the index of the important columns in the database
int colIndex[5][5] = {
    
    { 1, 3, 19, 20, 21},
    { 4, 6, 7,   0,  0},
    { 1, 3, 4,   0,  0},
    { 1, 2, 3,   0,  0},
    { 1, 3, 4,   5,  0}
};
// Names of the databases that we are using
char *dbNames[] = {"Clean-Watershed", "Health Corner Stores", "PPR Playgrounds", "PPR Parks", "PPR Recreation Facilities"};

void create(int index);
char ** dbJson;
void delete(int index);
void displayMenu();
char * parseCsv(char* fileName, char * dbName, int index );
void sig_handler(int sig);
void sigchld_handler(int sig);
void requestHandler(int * dbReq, char*** req, int numIds);

void stub(char*, int);


//main - The application's main routine
//int main(int argc, char **argv)
int main(void)
{
	int user_input = -1;		//a +/- integer value for database choice
	int add_remove = 0;
    int index = 0;
    char* raw_input = (char*)malloc( 5 * sizeof(char)) ;
    
    int i; // iterator
    
    int dbRequest[MAX_DATABASE]; // Stores the users request for the databases to be displayed
    char** objectIDs = malloc(MAX_DATABASE * sizeof(char*)); // Stores the current Object IDs that are submited in DB, for deletion

    signal(SIGINT,  sig_handler);   /* ctrl-c */
    signal(SIGTSTP, sig_handler); /* ctrl-z */
    signal(SIGQUIT, sig_handler); /* ctrl-\ */
    signal(SIGCHLD, sigchld_handler);
    
    // Init all of the Users dbRequests to zero
    // and all of the objectIds to NULL
    for ( i = 0; i < MAX_DATABASE; i++){
        dbRequest[i] = 0;
        objectIDs[i] = NULL;
       // printf("dbReq %d", dbRequest[i]);
    }
    
    //dbJson holds all json strings
    dbJson = malloc( MAX_DATABASE * sizeof(char*));
    
    //parses csv data files into Json and returns to dbJson
    for ( i = 0; i < MAX_DATABASE; i++ ){
        
        dbJson[i] = parseCsv(dbFileNames[i], dbNames[i], i);
       // printf(" THIS IS STRING: %s\n", dbJson[i]);
    }
		  
	//Execute the read/eval loop
	while (user_input != 0) {
        
        //Displaying menu
        displayMenu(dbRequest);
        //Get input from user
        scanf("%s", raw_input);
        
        //Check if the user is ready to view the inputs
        if ( strcmp(raw_input, "view") == 0 ){
            
           // printf("this is view");
            //Call the function that will spawn the process for threading
            requestHandler(dbRequest, &objectIDs, MAX_DATABASE);
            
        }else{ // convert the string to an integer and carry on
            user_input = atoi(raw_input);
        
            //Decrement the user input because
            index = abs(user_input) - 1 ;
        
            switch (abs(user_input)) {
                case 0:
                    break;
                case 1:
                    dbRequest[index] = user_input;
                    break;
                case 2:
                    dbRequest[index] = user_input;
                    break;
                case 3:
                    dbRequest[index] = user_input;
                    break;
                case 4:
                    dbRequest[index] = user_input;
                    break;
                case 5:
                    dbRequest[index] = user_input;
                    break;
                default:
                    printf("Please enter the correct input.");
                    fflush(stdout);
                    break;
            }// switch
        }
	}//while
	return 0;
}// main

void displayMenu(int * dbReq){
    
    int i; //iterator
    char *status;
    //Intro
    printf("\nTHE GREEN PHILLY PROJECT\n\n");
    printf("This app enables you to add and remove pins in a Google map.\n");
    printf("The pins represent data related to a 'green' Philly theme.\n");
    printf("You add or remove data from the map by entering +/- a number.\n");
    printf("The number represents one of the databases in the application.\n");
    printf("Presently, the following databsaes are available:\n\n");
    
    for ( i = 1; i <= MAX_DATABASE; i++ ){
        // Determines if the user has already selected this database
        int index = i - 1;
        if ( dbReq[index] <= 0 ){
            status = "Off";
        }else{
            status = "On";
        }
        printf("%d -- %s    (%s)\n", i, dbNames[index], status);
    }
        printf("\nYou can add or remove databases at your discretion.\n");
        printf("Once you have made your desired selections enter 'view'\n");
        printf("To exit, type 0 (zero) to quit the application.\n");
}

// Takes name of file, name of database, database macro
char* parseCsv(char* fileName, char * dbName, int index )
{
    const char* toks;
    int i; //iterator
    int j; //iterator
    int k; //iterator for parallel
    char* res; // response string of Json
    char line[1024]; // Buffer to hold each line of file
    cJSON *json, *root, *fmt; //cJson library elements
    
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
                
                if ( j == colIndex[index][k] && j != 0){
                    cJSON_AddStringToObject(fmt, colNames[index][k], toks);
                    k++;
                }
            }
        }
    }
    
    res = cJSON_Print(root);
    
    cJSON_Delete(root);
    
    fclose(stream);
    
    return res;
}


void stub(char * user_choice, int add_rem) {
    
	if(add_rem == 1)
		printf("User wants to add data ");
	else
		printf("User wants to remove data ");
	printf("from %s\n", user_choice);
    
}

void requestHandler(int * dbReq, char *** ids, int numIds){
    
    int i;
    pid_t pid;
    
    sigset_t mask;
    
	sigemptyset (&mask);
    sigaddset (&mask, SIGINT);
    sigaddset (&mask, SIGTSTP);
    
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask");
    }
    
    if ( (pid = fork()) == 0 ){
        // read the request
        setpgid(0,0);
        
        if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
                perror ("sigprocmask");
        }
        
        (*ids)[2] = "2342fafsd";
        
        for ( i = 0; i < MAX_DATABASE; i++ ){
           
            // if create and isnt already in database
            if ( dbReq[i] > 0 && (*ids)[i] == NULL ){
                
                (*ids)[i] = createDBentry(dbJson[i]);
            }
            // if delete and is still in database
            else if ( dbReq[i] < 0 && (*ids)[i] != NULL ){
                
                deleteDBentry((*ids)[i]);
                (*ids)[i] = NULL;
            
            }else{
                printf("This is wrong\n");
            }
        }
        
        //sleep(10);
    
        displayMenu(dbReq);
        
        exit(0);
    }
    
    return;
}

void create(int index){
    printf( " YOU HAVE CREATED %s\n", dbNames[index] );
    return;
}
void delete(int index){
    printf( " YOU HAVE Deleted %s\n", dbNames[index] );
    return;
}

/*
 * Signal Handlers
 *
 * Blocks all calls to kill the program
 * so the databases can be cleaned up
 */

void sig_handler(int sig){
    int i;
    int dbReq[MAX_DATABASE];
    
    printf("CLEAR ALL OF THE DATABASES\n");
    for ( i = 0; i < MAX_DATABASE; i++ ){
        dbReq[i] = 0;
    }
    requestHandler(dbReq, NULL, MAX_DATABASE);
    
    exit(0);
}

void sigchld_handler(int sig){
    
    printf("One of your children has closed, time to reap");
    
    pid_t pid;
    int status;
    struct job_t *job;
    
    if( sig == SIGCHLD){
        
        while( (pid = waitpid(-1, &status, WUNTRACED | WNOHANG)) > 0){
            
            kill(-pid, SIGKILL);
            
            /*if (WIFEXITED(status)){
                //  printf("THIS IS PID: %d\n", pid);
                //               printf("THIS IS HERE");
                //if (deletejob(jobs, pid) == 0 )
                    //printf("Child Hanlder: could not find job to delete");
                kill(-pid, SIGKILL);
            } else if( WIFSTOPPED(status)) {
               // if ( (job = getjobpid(jobs, pid)) == NULL)
                printf("Child Hanlder: Failed to find job");
                // printf("HAS BEEN STOPPED HERE");
                //job->state = ST;
            } else if (WIFCONTINUED(status)){
                //if ( (job = getjobpid(jobs, pid)) == NULL)
                    printf("Child Hanlder: Failed to find job");
                  //  job->state = BG;
                kill(pid, SIGCONT);
            }*/
        }
    }
    
    return;
}

