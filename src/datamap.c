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
#include <omp.h>
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

#define num_threads 4
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
    
    {   "name", "description",         "url","longitude",    "latitude"},
    {   "name",     "address",     "zipcode", "latitude",    "longitude"},
    {   "name",     "address",     "zipcode", "latitude",    "longitude"},
    {"address", "description",        "name", "latitude",    "longitude"},
    {   "name",     "address", "description", "latitude",    "longitude"}
};
// defines multi-array for the index of the important columns in the database
int colIndex[5][5] = {
    
    { 1, 3, 19, 20, 21},
    { 4, 6, 7,   9,  10},
    { 1, 3, 4,   5,  6},
    { 1, 2, 3,   4,  5},
    { 1, 3, 5,   6,  7}
};
// Names of the databases that we are using
char *dbNames[] = {"Clean-Watershed", "Health-Corner-Stores", "PPR-Playgrounds", "PPR-Parks", "PPR-Recreation-Facilities"}; 

void clearDB(); // used to clear all DB
char ** dbJson; // stores the json strings for each file
void displayMenu(); // displays a menu to the user
char * parseCsv(char* fileName, char * dbName, int index ); // parses csv files into json
void sig_handler(int sig); // handles int, stp,quit to clear dbs and then quit
void sigchld_handler(int sig); // handles reaping the child
void requestHandler(int * dbReq, char*** req, int numIds); // handles forking child and to load the json string to the database

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
    
    //omp_set_num_threads(num_threads);

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
        //printf(" THIS IS STRING: %s\n", dbJson[i]);
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
            // Adjust the dbReq array
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
    
    clearDB();
    
	return 0;
}// main
// Displays the menu for the user
// lists the databases that are supported
// displays with of them are already loaded into the DB
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
    // iterate over the database names in the dbNames file
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
// Reads from a file, and iterates over the csv file taking the
// desired fields which are located in the colIndex global array
// and parses those into Json objects adding to 1 json file per
// file and returns that object
char* parseCsv(char* fileName, char * dbName, int index )
{
    const char* toks; // used to iterate over each item in line
    int i; //iterator
    int j; //iterator
    int k; //iterator for parallel
    char* res; // response string of Json
    char line[1024]; // Buffer to hold each line of file
    cJSON *json, *root, *fmt; //cJson library elements
    
    //opening file
    FILE* stream = fopen(fileName, "r");
    
    // create the root object for cJSON
    root=cJSON_CreateObject();
	//Create the first object is an array
	cJSON_AddItemToObject(root, dbName, json=cJSON_CreateArray());
   
    //iterates over each line in the file
    for( i = 0; fgets(line, 1024, stream) != NULL; i++ )
    {
        // if its not the header file
        if ( i != 0 ){
            //adding the first object
            cJSON_AddItemToObject(json, "location", fmt=cJSON_CreateObject());
            //toks the line removing newline and seeding strtok
            toks = strtok(line, "\n,");
            
            k = 0;
            // iterates over each item in the line of the csv
            for( j = 0; toks !=NULL ; toks = strtok(NULL, "\n,\r"), j++){
                // if its an important col
                if ( j == colIndex[index][k] && j != 0){
                    //add that col to the object
                    cJSON_AddStringToObject(fmt, colNames[index][k], toks);
                    //increment the index for the colNames
                    k++;
                }
            }
        }
    }
    // assign res the Json String
    res = cJSON_Print(root);
    // cleaning up the root
    cJSON_Delete(root);
    // closing the stream
    fclose(stream);
    
    return res;
}

// For the glory
// NOT USED IN PROGRAM
void stub(char * user_choice, int add_rem) {
    
	if(add_rem == 1)
		printf("User wants to add data ");
	else
		printf("User wants to remove data ");
	printf("from %s\n", user_choice);
    
}

// Handles the user request for the DB that will be displayed on the website
// params: dbReq which is an array of the databases needed
void requestHandler(int * dbReq, char *** ids, int numIds){
    
    int i;
    pid_t pid; // pid of child process
    
    sigset_t mask;
    
	sigemptyset (&mask); // creates empty mask
    sigaddset (&mask, SIGINT); // pushes signal set on mask
    sigaddset (&mask, SIGTSTP); // pushes signal set on mask
    
    // Blocks all signal handling
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask");
    }
    
    if ( (pid = fork()) == 0 ){
        //sets the group id of the child process to its pid
        setpgid(0,0);
        // unblock sigs
        if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
                perror ("sigprocmask");
        }
        
        //PRAGMA
	#pragma omp parallel for
        for ( i = 0; i < MAX_DATABASE; i++ ){
           
            // if create and isnt already in database
            if ( dbReq[i] > 0 && (*ids)[i] == NULL ){
                
                (*ids)[i] = createDBentry( dbJson[i] );
                (*ids)[i] = dbNames[i];
               // printf("This is the key %s", (*ids)[i]);
            }
            // if delete and is still in database
            else if ( dbReq[i] < 0 ){//&& (*ids)[i] != NULL ){
               // printf("This is the key %s", dbNames[i]);
                deleteDBentry( dbNames[i] );
                (*ids)[i] = NULL;
            }
        }
        
        //SYSTEM COMMAND opens a browser on the system
        system("open http://www2.cs.drexel.edu/~sc3356/googMap/special/");
        
        exit(0);
        
    }// end of fork
    
    // unblocks the signals returns to original state
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
        perror ("sigprocmask");
    }
    
    // Displays the menu for the user to alter the database entries
    displayMenu(dbReq);
    
    return;
}
//Calls deleteDBentry on all entrys in the DB
void clearDB(){
    int i;
    
    //prgama
    #pragma omp parallel for
    for ( i = 0; i < MAX_DATABASE; i++){
        deleteDBentry(dbNames[i]);
    }
    
}

/*
 * Signal Handlers
 *
 * Blocks all calls to kill the program
 * so the databases can be cleaned up
 */

//Clears all the records in the database clearing the map
void sig_handler(int sig){
    
    clearDB();
    
    exit(0);
}

// Handles reaping the child process used for sending data to the server
void sigchld_handler(int sig){
    
    //printf("One of your children has closed, time to reap");
    
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

