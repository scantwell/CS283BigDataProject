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

//Global
#define MAX_DATABASE 5 // Max databases
char *dbNames[] = {"Clean-Watershed", "Health Corner Stores", "PPR Playgrounds", "PPR Parks", "PPR Recreation Facilities"};

void displayMenu();
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
    char** objectIDs[MAX_DATABASE]; // Stores the current Object IDs that are submited in DB, for deletion

    signal(SIGINT,  sig_handler);   /* ctrl-c */
    signal(SIGTSTP, sig_handler); /* ctrl-z */
    signal(SIGQUIT, sig_handler); /* ctrl-\ */
    signal(SIGCHLD, sigchld_handler);
    
    // Init all of the Users dbRequests to zero
    // and all of the objectIds to NULL
    for ( i = 0; i < MAX_DATABASE; i++){
        dbRequest[i] = 0;
        objectIDs[i] = NULL;
        printf("dbReq %d", dbRequest[i]);
    }
		  
	//Execute the read/eval loop
	while (user_input != 0) {
        
        //Displaying menu
        displayMenu(dbRequest);
        //Get input from user
        scanf("%s", raw_input);
        
        //Check if the user is ready to view the inputs
        if ( strcmp(raw_input, "view") == 0 ){
            
            printf("this is view");
            //Call the function that will spawn the process for threading
            requestHandler(dbRequest, (char***)&objectIDs, MAX_DATABASE);
            
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

void stub(char * user_choice, int add_rem) {
    
	if(add_rem == 1)
		printf("User wants to add data ");
	else
		printf("User wants to remove data ");
	printf("from %s\n", user_choice);
    
}
void requestHandler(int * dbReq, char *** ids, int numIds){
    
  //  pid_t = pid;
    
    //if ( (pid = fork()) == 0 ){
        // read the request
    //}
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
    
}
