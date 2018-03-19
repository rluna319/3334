#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

//-----------------------SIC Commands--------------------------//

void loadf (char *);
void execute (char *);
void debug ();
void dump (char *, char *);
void help ();
void assemble (char *);


//-----------------------Structures + Functions---------------//  

//						--Start OpTable--
struct OpTable
{
	char instruction[6];		//instruction
	short opcode;				//opcode
	char directive[6];			//directive operand
};
struct OpTable OPTAB[31];	//OpTable array (31 total operands)

//builds optable
void OpTable();

//						--End OpTable--

//						--Start SymbolTable--
struct SymbolTable
{
	char label[6];		//label (max = 6 chars)(alphanumeric)
	int address;		//label address
	int count;			//tracks # of labels being inserted into the table
};
struct SymbolTable SYMTAB [500];	//SymbolTable array (max = 500 labels)

//initializes address value to -1
//void SymbolTable();

//						--End SymbolTable--

//						--Start ErrorFlag--
struct ErrorFlag {char output[100];};	
struct ErrorFlag Error[9]; //ErrorFlag array of strings (total of 9 possible errors in phase2)

	void ErrorFlag();	//initializes Error definitions

	void Error0(){printf("%s\n", Error[0].output);}
	void Error1(){printf("%s\n", Error[1].output);}
	void Error2(){printf("%s\n", Error[2].output);}
	void Error3(){printf("%s\n", Error[3].output);}
	void Error4(){printf("%s\n", Error[4].output);}
	void Error5(){printf("%s\n", Error[5].output);}
	void Error6(){printf("%s\n", Error[6].output);}
	void Error7(){printf("%s\n", Error[7].output);}
	void Error8(){printf("%s\n", Error[8].output);}
	//void Error9(){printf("%s\n", Error[9].output);}
	//void Error10(){printf("%s\n", Error[10].output);}

//						--End ErrorFlag--

//-----------------------File Declarations--------------------//

FILE *source, *intermediate, *symboltable;


//-----------------------Program Functions--------------------//  

//splits char array (string)
void split (char *, char *, char *, char *, int *);


//-----------------------Global Variables---------------------//

int location; //location counter
int start;	//program start location
int progLen;	//legnth of program
int end;	//program end location
char progName[20];	//name of program
char token[500];	//token string
char line[500];		//stores line in file
char delimiter[2];	//holds delimiters for tokenizing source line


//------------------------Program-----------------------------//  

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
		split(str, cmd, prm1, prm2, &n);
		
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

void split (char *str, char *cmd, char *prm1, char *prm2, int *n)
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
	//Open source file for reading
	source fopen(prm1, "r");
	if (source == NULL)		//file not located (stop loadf)
	{
		printf ("%s does not exist.\nPlease try again.", prm1);
		printf ("Remember that filenames are case-sensitive...\n\n");
		return 0;
	}
	else printf ("%s successfully opened!\n", prm1);	//file located (status print)

	//Open intermediate and symbol table files for writing
	intermediate fopen("Intermediate.txt", "w");
	symboltable fopen("SymbolTable.txt", "w");
	if (intermediate == NULL) {Error9();}	//unable to open file for writing
	else printf ("\n'Intermediate.txt' file created...\n");
	if (symboltable == NULL) {Error9();}	//unable to open file for writing
	else printf ("'SymbolTable.txt' file created...\n");

	//status print
	printf("\n	Beginning Pass 1...\n");

	//initialize OpTable and delimiters
	OPTAB();
	delimiter[0] = " ";
	delimiter[1] = "\t";

	//initial print
	fprintf(intermediate, "Intermediate File\n");
	fprintf(intermediate, "--lists source line, location counter, mnemonics, operands, and errors--\n\n");

	//local variables
	int i = 0;

	while(fgets(line, 500, source))
	{
		location += 1;
		strtok(line, delimiter);
	}

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

//build/initialize OpTable
void OpTable()
{
	strcpy(OPTAB[0].instruction, "ADD");
	OPTAB[0].opcode = 0x8;
	strcpy(OPTAB[0].directive, "");

	strcpy(OPTAB[1].instruction, "AND");
	OPTAB[1].opcode = 0x58;
	strcpy(OPTAB[1].directive, "");

	strcpy(OPTAB[2].instruction, "COMP");
	OPTAB[2].opcode = 0x28;
	strcpy(OPTAB[2].directive, "");

	strcpy(OPTAB[3].instruction, "DIV");
	OPTAB[3].opcode = 0x24;
	strcpy(OPTAB[3].directive, "");

	strcpy(OPTAB[4].instruction, "J");
	OPTAB[4].opcode = 0x3C;
	strcpy(OPTAB[4].directive, "");

	strcpy(OPTAB[5].instruction, "JEQ");
	OPTAB[5].opcode = 0x30;
	strcpy(OPTAB[5].directive, "");

	strcpy(OPTAB[6].instruction, "JGT");
	OPTAB[6].opcode = 0x34;
	strcpy(OPTAB[6].directive, "");

	strcpy(OPTAB[7].instruction, "JLT");
	OPTAB[7].opcode = 0x38;
	strcpy(OPTAB[7].directive, "");

	strcpy(OPTAB[8].instruction, "JSUB");
	OPTAB[8].opcode = 0x48;
	strcpy(OPTAB[8].directive, "");

	strcpy(OPTAB[9].instruction, "LDA");
	OPTAB[9].opcode = 0x00;
	strcpy(OPTAB[9].directive, "");

	strcpy(OPTAB[10].instruction, "LDCH");
	OPTAB[10].opcode = 0x50;
	strcpy(OPTAB[10].directive, "");

	strcpy(OPTAB[11].instruction, "LDL");
	OPTAB[11].opcode = 0x08;
	strcpy(OPTAB[11].directive, "");

	strcpy(OPTAB[12].instruction, "LDX");
	OPTAB[12].opcode = 0x04;
	strcpy(OPTAB[12].directive, "");

	strcpy(OPTAB[13].instruction, "MUL");
	OPTAB[13].opcode = 0x20;
	strcpy(OPTAB[13].directive, "");

	strcpy(OPTAB[14].instruction, "OR");
	OPTAB[14].opcode = 0x44;
	strcpy(OPTAB[14].directive, "");

	strcpy(OPTAB[15].instruction, "RD");
	OPTAB[15].opcode = 0xD8;
	strcpy(OPTAB[15].directive, "");

	strcpy(OPTAB[16].instruction, "RSUB");
	OPTAB[16].opcode = 0x4C;
	strcpy(OPTAB[16].directive, "");

	strcpy(OPTAB[17].instruction, "STA");
	OPTAB[17].opcode = 0x0C;
	strcpy(OPTAB[17].directive, "");

	strcpy(OPTAB[18].instruction, "STCH");
	OPTAB[18].opcode = 0x54;
	strcpy(OPTAB[18].directive, "");

	strcpy(OPTAB[19].instruction, "STL");
	OPTAB[19].opcode = 0x14;
	strcpy(OPTAB[19].directive, "");

	strcpy(OPTAB[20].instruction, "STX");
	OPTAB[20].opcode = 0x10;
	strcpy(OPTAB[20].directive, "");

	strcpy(OPTAB[21].instruction, "SUB");
	OPTAB[21].opcode = 0x1C;
	strcpy(OPTAB[21].directive, "");

	strcpy(OPTAB[22].instruction, "TD");
	OPTAB[22].opcode = 0xE0;
	strcpy(OPTAB[22].directive, "");

	strcpy(OPTAB[23].instruction, "TIX");
	OPTAB[23].opcode = 0x2C;
	strcpy(OPTAB[23].directive, "");

	strcpy(OPTAB[24].instruction, "WD");
	OPTAB[24].opcode = 0xDC;
	strcpy(OPTAB[24].directive, "");

	strcpy(OPTAB[25].instruction, "START");
	strcpy(OPTAB[25].directive, "START");

	strcpy(OPTAB[26].instruction, "END");
	strcpy(OPTAB[26].directive, "END");

	strcpy(OPTAB[27].instruction, "BYTE");
	strcpy(OPTAB[27].directive, "BYTE");

	strcpy(OPTAB[28].instruction, "WORD");
	strcpy(OPTAB[28].directive, "WORD");

	strcpy(OPTAB[29].instruction, "RESB");
	strcpy(OPTAB[29].directive, "RESB");

	strcpy(OPTAB[30].instruction, "RESW");
	strcpy(OPTAB[30].directive, "RESW");
}

void ErrorFlag()
{
	strcpy(Error[0].output, " |Error: Duplicate label found| ");
	strcpy(Error[1].output, " |Error: Illegal label| ");
	strcpy(Error[2].output, " |Error: Illegal operation| ");
	strcpy(Error[3].output, " |Error: Missing or illegal operand on data storage directive| ");
	strcpy(Error[4].output, " |Error: Missing or illegal operand on START directive| ");
	strcpy(Error[5].output, " |Error: Missing or illegal operand on END directive| ");
	strcpy(Error[6].output, " |Error: Too many symbols in source program| ");
	strcpy(Error[7].output, " |Error: Program too long| ");
	strcpy(Error[8].output, " |Error: Unable to open file for writing| ");
}