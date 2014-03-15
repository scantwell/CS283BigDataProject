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
void stub(char*, int);


//main - The application's main routine
//int main(int argc, char **argv)
int main(void)
{
	int user_input = -1;		//a +/- integer value for database choice
	int add_remove = 0;
    int index = 0;
    
    int i; // iterator
	char * choice1 =
		"http://www.opendataphilly.org/opendata/resource/220/green-stormwater-projects/";
	char * choice2 =
		"http://www.opendataphilly.org/opendata/resource/216/healthy-corner-store-locations/";
	char * choice3 =
		"http://www.opendataphilly.org/opendata/resource/222/ppr-playgrounds/";
	char * choice4 =
		"http://www.opendataphilly.org/opendata/resource/237/ppr-parks/";
	char * choice5 =
		"http://www.opendataphilly.org/opendata/resource/231/ppr-recreation-facilities/";
    
    int dbRequest[MAX_DATABASE]; // Stores the users request for the databases to be displayed
    char** objectIDs[MAX_DATABASE]; // Stores the current Object IDs that are submited in DB, for deletion

    for ( i = 0; i < MAX_DATABASE; i++){
        dbRequest[i] = 0;
        printf("dbReq %d", dbRequest[i]);
    }
		  
	//Execute the read/eval loop
	while (user_input != 0) {
        
        //Displaying menu
        displayMenu(dbRequest);
        //Get input from user
        scanf("%d", &user_input);
        
        //Decrement the user input because
        index = abs(user_input) - 1 ;
        
      //printf("User input %d \n", user_input);
      //printf("User index %d \n", index);
        
        switch (abs(user_input)) {
            case 0:
                break;
            case 1:
                dbRequest[index] = user_input;
                //stub(choice1, add_remove);
                break;
            case 2:
                dbRequest[index] = user_input;
                //stub(choice2, add_remove);
                break;
            case 3:
                dbRequest[index] = user_input;
                //stub(choice3, add_remove);
                break;
            case 4:
                dbRequest[index] = user_input;
                //stub(choice4, add_remove);
                break;
            case 5:
                dbRequest[index] = user_input;
                //stub(choice5, add_remove);
                break;
            default:
                printf("Please enter the correct input.");
                //fflush(stdout);
                break;
        }// switch
	}//while
	return 0;
}// main

void stub(char * user_choice, int add_rem) {
	if(add_rem == 1)
		printf("User wants to add data ");
	else
		printf("User wants to remove data ");
	printf("from %s\n", user_choice);
}

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
