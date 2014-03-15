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

void stub(char * user_choice, int add_rem) {
	if(add_rem == 1)
		printf("User wants to add data ");
	else
		printf("User wants to remove data ");
	printf("from %s\n", user_choice);
}

//main - The application's main routine
//int main(int argc, char **argv)
int main(void)
{
	int user_input = -1;		//a +/- integer value for database choice
	int add_remove = 0;
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

	//Print instructions to user in stdout
	printf("\nTHE GREEN PHILLY PROJECT\n\n");
	printf("This app enables you to add and remove pins in a Google map.\n");
	printf("The pins represent data related to a 'green' Philly theme.\n"); 
	printf("You add or remove data from the map by entering +/- a number.\n");
  	printf("The number represents one of the databases in the application.\n");
	printf("Presently, the following databsaes are available:\n\n");
	printf("1 -- Clean Watershed\n");
	printf("2 -- Health Corner Stores\n");
	printf("3 -- PPR Playgrounds\n");
	printf("4 -- PPR Parks\n");
	printf("5 -- PPR Recreation Facilities\n");
	printf("\nYou can add or remove databases at your discretion.\n");
	printf("When finished, type 0 (zero) to quit the application.\n");
		

	//Execute the read/eval loop
	while (scanf("%d", &user_input) != 0) {
		if(user_input == 0) {
			printf("\nGoodbye\n");
			return 1;
		}
		else
		{
			system("/usr/bin/firefox http://www.google.com");
#pragma omp parallel for
			if(user_input > 0)
				add_remove = 1;
			else
				add_remove = -1;
				
			switch (abs(user_input)) {
				case 1:
					stub(choice1, add_remove);
					break;
				case 2:
					stub(choice2, add_remove);
					break;
				case 3:
					stub(choice3, add_remove);
					break;
				case 4:
					stub(choice4, add_remove);
					break;
				case 5:
					stub(choice5, add_remove);
					break;
			}
		}
	}

	return 0;
}
