#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

//-------------------------------------------------------------//

void breakit (char *, char *, char *, char *, int *);
void loadf (char *);
void execute (char *);
void debug ();
void dump (char *, char *);
void help ();
void assemble (char *);

//-------------------------------------------------------------//  

int main()
{

	char str[80], cmd[10], prm1[20], prm2[20];
	int length, n;
	bool exit = false;
		
	//greeting
        printf ("Greetings User! Please enter a command: \n");


	//loop checking if 'exit' command is entered
	while(exit == false)
	{
		//reset word count
		n = 0;
	
		printf("$$$: ");
	
		//get input	
        	fgets (str, 80, stdin);

		//get rid of trailing newline
		length = strlen(str) - 1;

		if (str[length] == '\n') {str[length] = '\0';}

		//break up input
		breakit(str, cmd, prm1, prm2, &n);
		
		//exit
		if (strcmp(cmd, "exit") == 0)
		{
			if (n == 1) 
			{exit = true;}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}
		
		//call help function
		else if (strcmp(cmd, "help") == 0)
		{
			if (n == 1)
			{
				printf ("\n");
				help();
				printf ("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}
		
		//call directory function
		else if (strcmp(cmd, "dir") == 0 || strcmp(cmd, "directory") == 0)
		{
			if (n == 1)
			{
				printf ("\n");
				system("ls");
				printf ("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}
				
		//call load function
		else if (strcmp(cmd, "load") == 0) 
		{
			if (n == 2)
			{
				printf ("\n");
				loadf(prm1);
				printf ("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
 		}
		
		//call execute funtion
		else if (strcmp(cmd, "execute") == 0)
		{
			if (n == 2)
			{
				printf ("\n");
				execute (prm1);
				printf ("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}

		//call debug funtion
		else if (strcmp(cmd, "debug") == 0)
		{	
			if (n == 1)
			{
				printf("\n");
				debug ();
				printf("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}

		//call dump function
		else if (strcmp(cmd, "dump") == 0)
		{
			if (n == 3)
			{
				printf("\n");
				dump (prm1, prm2);
				printf("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}

		//call assemble funtion
		else if (strcmp(cmd, "assemble") == 0)
		{
			if (n == 2)
			{
				printf("\n");
				assemble(prm1);
				printf("\n");
			}
			else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
		}
		else printf ("Invalid entry. Type 'help' for a list of commands.\n\n");
	}
	

	return 0;
}

//-------------------------------------------------------------//

void breakit (char *str, char *cmd, char *prm1, char *prm2, int *n)
{
	//set index 0 of char variables to null
	cmd[0] = prm1[0] = prm2[0] = '\0';
	
	//index variable and word counter
	int x = 0, wrdcount = 0;

	//booleans to flag if word is complete
	bool cmdF = false, prm1F = false, prm2F = false;
	
	for(int i = 0; i < 80; i++)
	{	
		//break if newline char or null
		if(str[i] == '\n' || str[i] == '\0')
		{
			if (cmd[0] != '\0') wrdcount++; 	
			break;
		}
		
		//if ' ' is hit flag a word and reset index variable
		if(str[i] == ' ') 
		{
			if(cmd[0] != '\0')
			{
				cmdF = true;
			}
			if(prm1[0] != '\0')
			{	
				prm1F = true;
			} 
			if(prm2[0] != '\0')
			{
				prm2F = true;
			}
			
			//index reset
			x = 0;

			//increment word count	
			wrdcount++;
			
			continue;
		}
		
		//if current char in str isnt a space then add it to the corresponding word
		//after the insertion move null value to the right
		else if(str[i] != ' ') 
		{
			if(cmdF == false)
			{
				cmd[x] = str[i];
				x++;
				cmd[x] = '\0';
				continue;
			}
			if(prm1F == false) 
			{
				prm1[x] = str[i];
				x++;
				prm1[x] = '\0';
				continue;
			}
			if(prm2F == false)
			{
				prm2[x] = str[i];
				x++;
				prm2[x] = '\0';
				continue;
			}
		}		
	}
	
	//update n pointer
	*n = wrdcount;

	//					Test
	//	printf ("-------------------------------------------------------------------\n");
    	//	printf ("command: %s \nparameter1: %s \nparameter2: %s \nword count: %i \n", cmd, prm1, prm2, *n);
	//	printf ("-------------------------------------------------------------------\n");
}

void loadf(char *prm1)
{
	printf ("'load' command recognized! \n");
	printf ("Parameter: %s \n", prm1);
}

void execute (char *prm1)
{
	printf ("'execute' command recognized! \n");
	printf ("Parameter: %s \n", prm1);
}

void debug()
{
	printf ("'debug' command recognized! \n");
}

void dump (char *prm1, char *prm2)
{
	printf ("'dump' command recognized! \n");
	printf ("Parameter 1: %s \nParameter 2: %s \n", prm1, prm2);
}

void help()
{
	printf ("----------------------------------------------------------------------------------------------------------\n");
	printf ("			Here is a list of commands you may enter: \n\n");
	printf ("	~ load <file> - Loads a specified file. \n");
	printf ("	~ execute <program> - Calls the computer simulation program to execute the program that was previously loaded in memory. \n");
	printf ("	~ debug - Starts the debugger. \n");
	printf ("	~ dump <starting address (hexadecimal)> <ending address (hexadecimal)> - Dumps the memory in the specified range. \n");
	printf ("	~ help - Lists available commands. \n");
	printf ("	~ directory - (dir) Lists files within your directory. \n");
	printf ("	~ exit - This will exit the simulator. Make sure to save your work! \n");  
	printf ("----------------------------------------------------------------------------------------------------------\n");
}

void assemble(char *prm1)
{
	printf ("'assemble' command recognized! \n");
	printf ("Parameter: %s \n", prm1);
}

