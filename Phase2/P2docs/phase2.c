//Ryan Luna
//3334-02
/*
		
				Program Description

	This is phase 2 of my SIC (Simplified Instructional Computer) simulator.
While phase 1 got the commands up and running. Phase 2 now implements pass 1 of 
the 2 pass assembler. Pass 1 will use the assemble command to generate a symbol 
table and an intermediate file. My intermediate file will contain the source line,
program counter, label, instruction, operand, and errors generated. The following
are the names of files being generated... 
	
	Symbol Table: 'SymbolTable.txt' 
	Intermediate File: 'Intermediate.txt'
	
       					end
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

//-----------------------SIC Commands--------------------------//

void loadf (char *);
void execute ();
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
	char label[6];		//label (max = 6 chars)(alphadigitseric)
	long address;		//label address
};
struct SymbolTable SYMTAB[500];	//SymbolTable array (max = 500 labels)
struct SymbolTable *SYM = 0;	//SYMTAB pointer initailized to nothing

//						--End SymbolTable--

//						--Start Token--
struct Token 
{
	char str[500];		//token
	bool hastoken;		//has token?
};
struct Token token[4];	//array Token (max of 4 tokens per line)

//						--End Token--

//						--Start ErrorFlags--
struct ErrorFlags {char output[100];};	
struct ErrorFlags Error[21]; //ErrorFlags array of strings (total of 21 possible errors in phase2)

void ErrorFlags();	//initializes Error definitions

//						--End ErrorFlags--

//-----------------------File Declarations--------------------//

FILE *source, *intermediate, *symboltable, *Errors, *obj;


//-----------------------Program Functions--------------------//  

//splits char array (string)
void split (char *, char *, char *, char *, int *);
//Taks a string and parses it into tokens
void Tokenize(char *);
//Clears token[]
void T_clear();
//Pass 1 (getting errors and writing to the 'symboltable' and 'intermediate' files)
void Pass1();
//for easy oneliner conversion from string to hex
long toHex(char);
//insert label into SYMTAB
void symInsert(char *, long);
//Clears SYMTAB[]
void S_clear();
//search funtion for symbol table
bool symSearch(char *);
//function to process index operands
void indexOp(char *);
//gets substring of a char array (string)
char* substr(char*, int, int);
//converts instruction into opcode and checks if it's in optable
void OpConvert(char *);


//-----------------------Global Variables---------------------//

int progStart = 0;	//program start address
long locctr = 0;	//program counter
int progLen = 0;	//legnth of program
char progName[20];	//name of program
char line[500];		//stores line in file
int tcount = 0;		//token counter
int symI = 0;		//SYMTAB indexer/counter
int ErrorCount = 0;	//Error count
int SymCount = 0;	//# of Symbols
char Sname[20];		//stores source file name
char I_fname[20] = "Intermediate.txt";	//Intermediate filename
char ST_fname[20] = "SymbolTable.txt";	//SymbolTable filename


//------------------------Main--------------------------------//  

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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
 		}
		
		//call execute funtion
		else if (strcmp(cmd, "execute") == 0)
		{
			if (n == 1)
			{
				printf ("\n");
				execute();
				printf ("\n");
			}
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
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
			else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
		}
		else printf ("\nInvalid entry. Type 'help' for a list of commands.\n\n");
	}
	

	return 0;
}

//-----------------------Function Definitions-------------------//  

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
	printf("Load command recognized...\n");
}

void execute ()
{
	printf("Execute command recognized...\n");
	
	//ADDRESS eAddr = GetPC();

	//SICRun(&eAddr, 0);

	printf("Execution Complete!\n");
}

void dhelp()
{
	printf("\n\t\tHere is a list of the debugger commands...\n");
	printf("\n\t'h' ~ Print out help menu.");
	printf("\n\t'p' ~ Print contents of all registers.");
	printf("\n\t'c' ~ Change contents of a register or memory location.");
	printf("\n\t's' ~ Step forward.");
	printf("\n\t'r' ~ Run the entire program.");
	printf("\n\t'q' ~ Quit debugging.\n\n");
}

void debug()
{
	printf("Debug command recognized...\n");
}

void dump (char *prm1, char *prm2)
{
	printf("Dump command recognized...\n");
}

void help()
{
	printf ("\n");
	printf ("			Here is a list of commands you may enter: \n\n");
	printf ("~ load <file> \n");
	printf ("	- Loads a specified file. \n\n");
	printf ("~ execute <program> \n");
	printf ("	- Calls the computer simulation program to execute the program that was\n\tpreviously loaded in memory. \n\n");
	printf ("~ debug \n");
	printf ("	- Starts the debugger. \n\n");
	printf ("~ dump <starting address (hexa)> <ending address (hex)> \n");
	printf ("	- Starts the debugger. \n\n");
	printf ("~ help \n");
	printf ("	- Lists available commands. \n\n");
	printf ("~ directory (dir)\n");
	printf ("	- Lists files within your directory. \n\n");
	printf ("~ exit \n");
	printf ("	- This will exit the simulator. Make sure to save your work! \n\n"); 
	printf ("~ assemble \n");
	printf ("	- assembles the source file loaded. \n\n"); 
	printf ("\n");
}

void assemble(char *prm1)
{
	intermediate = fopen(I_fname, "w");
	symboltable = fopen(ST_fname, "w");
	source = fopen(prm1, "r");

	if (source)
	{
		if (intermediate && symboltable){
			//status print
			printf("%s created in current directory...\n", I_fname);
			printf("%s created in current directory...\n\n", ST_fname);
		}
		else {
			printf("Internal Error: Cannot open intermediate and/or symbol table for writing...\n\n");
			return;
		}

		//initialize OpTable and line array
		OpTable();
		strcpy(line, "\0");

		//Begin Pass 1
		Pass1();
	}
	else {
		printf("	Error!\n	File not found. Filenames are case sensitive!\n\n");
		printf("	-Remember to include the file in the same directory as the program...\n\n");
		return;
	}	
}

void OpTable()
{
	strcpy(OPTAB[0].instruction, "ADD");
	OPTAB[0].opcode = 0x8;
	strcpy(OPTAB[0].directive, "\0");

	strcpy(OPTAB[1].instruction, "AND");
	OPTAB[1].opcode = 0x58;
	strcpy(OPTAB[1].directive, "\0");

	strcpy(OPTAB[2].instruction, "COMP");
	OPTAB[2].opcode = 0x28;
	strcpy(OPTAB[2].directive, "\0");

	strcpy(OPTAB[3].instruction, "DIV");
	OPTAB[3].opcode = 0x24;
	strcpy(OPTAB[3].directive, "\0");

	strcpy(OPTAB[4].instruction, "J");
	OPTAB[4].opcode = 0x3C;
	strcpy(OPTAB[4].directive, "\0");

	strcpy(OPTAB[5].instruction, "JEQ");
	OPTAB[5].opcode = 0x30;
	strcpy(OPTAB[5].directive, "\0");

	strcpy(OPTAB[6].instruction, "JGT");
	OPTAB[6].opcode = 0x34;
	strcpy(OPTAB[6].directive, "\0");

	strcpy(OPTAB[7].instruction, "JLT");
	OPTAB[7].opcode = 0x38;
	strcpy(OPTAB[7].directive, "\0");

	strcpy(OPTAB[8].instruction, "JSUB");
	OPTAB[8].opcode = 0x48;
	strcpy(OPTAB[8].directive, "\0");

	strcpy(OPTAB[9].instruction, "LDA");
	OPTAB[9].opcode = 0x00;
	strcpy(OPTAB[9].directive, "\0");

	strcpy(OPTAB[10].instruction, "LDCH");
	OPTAB[10].opcode = 0x50;
	strcpy(OPTAB[10].directive, "\0");

	strcpy(OPTAB[11].instruction, "LDL");
	OPTAB[11].opcode = 0x08;
	strcpy(OPTAB[11].directive, "\0");

	strcpy(OPTAB[12].instruction, "LDX");
	OPTAB[12].opcode = 0x04;
	strcpy(OPTAB[12].directive, "\0");

	strcpy(OPTAB[13].instruction, "MUL");
	OPTAB[13].opcode = 0x20;
	strcpy(OPTAB[13].directive, "\0");

	strcpy(OPTAB[14].instruction, "OR");
	OPTAB[14].opcode = 0x44;
	strcpy(OPTAB[14].directive, "\0");

	strcpy(OPTAB[15].instruction, "RD");
	OPTAB[15].opcode = 0xD8;
	strcpy(OPTAB[15].directive, "\0");

	strcpy(OPTAB[16].instruction, "RSUB");
	OPTAB[16].opcode = 0x4C;
	strcpy(OPTAB[16].directive, "\0");

	strcpy(OPTAB[17].instruction, "STA");
	OPTAB[17].opcode = 0x0C;
	strcpy(OPTAB[17].directive, "\0");

	strcpy(OPTAB[18].instruction, "STCH");
	OPTAB[18].opcode = 0x54;
	strcpy(OPTAB[18].directive, "\0");

	strcpy(OPTAB[19].instruction, "STL");
	OPTAB[19].opcode = 0x14;
	strcpy(OPTAB[19].directive, "\0");

	strcpy(OPTAB[20].instruction, "STX");
	OPTAB[20].opcode = 0x10;
	strcpy(OPTAB[20].directive, "\0");

	strcpy(OPTAB[21].instruction, "SUB");
	OPTAB[21].opcode = 0x1C;
	strcpy(OPTAB[21].directive, "\0");

	strcpy(OPTAB[22].instruction, "TD");
	OPTAB[22].opcode = 0xE0;
	strcpy(OPTAB[22].directive, "\0");

	strcpy(OPTAB[23].instruction, "TIX");
	OPTAB[23].opcode = 0x2C;
	strcpy(OPTAB[23].directive, "\0");

	strcpy(OPTAB[24].instruction, "WD");
	OPTAB[24].opcode = 0xDC;
	strcpy(OPTAB[24].directive, "\0");

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

void ErrorFlags()
{
	strcpy(Error[0].output, " |Error: Duplicate label found| ");
	strcpy(Error[1].output, " |Error: Illegal label| ");
	strcpy(Error[2].output, " |Error: Illegal operation| ");
	strcpy(Error[3].output, " |Error: Missing or illegal operand on data storage directive| ");
	strcpy(Error[4].output, " |Error: Missing or illegal operand on START directive| ");
	strcpy(Error[5].output, " |Error: Missing or illegal operand on END directive| ");
	strcpy(Error[6].output, " |Error: Too many symbols in source program| ");
	strcpy(Error[7].output, " |Error: Program too long| ");
	strcpy(Error[8].output, " |Error: Unable to open Intermediate.txt for writing| ");
	strcpy(Error[9].output, " |Error: Unable to open SymbolTable.txt for writing| ");

	//				start Pass 1 Errors
	strcpy(Error[10].output, " |Error: | ");
	strcpy(Error[11].output, " |Error: | ");
	strcpy(Error[12].output, " |Error: | ");
	strcpy(Error[13].output, " |Error: | ");
	strcpy(Error[14].output, " |Error: | ");
	strcpy(Error[15].output, " |Error: | ");
	strcpy(Error[16].output, " |Error: | ");
	strcpy(Error[17].output, " |Error: | ");
	strcpy(Error[18].output, " |Error: | ");
	strcpy(Error[19].output, " |Error: | ");
	strcpy(Error[20].output, " |Error: | ");


	//				stop Pass 1 Errors
}

void Tokenize(char *line)
{
	bool cont;		//continue loop?
	bool stop;		//early stop of line?
	int tok = 0; 	//index var for 'token[]'
	int i = 0;		//index var for 'line[]'

	T_clear();
	cont = true;
	stop = false;

	//if line is a comment do not tokenize
	if (line[0] == '.') {
		strcpy(token[3].str, line);
		return;
	}

	if (line[0] == ' ' || line[0] == '\t') tok = 1;

	//tokenize loop
	while (tok < 3)
	{
		cont = true;
		int begin = i;
		memset(token[tok].str, 0, 500);

		while (cont)
		{
			switch (line[i])
			{
			case ' ':
			case '\t':
			case '\r':
			case '\v':
			case '\0':	//if empty char is hit
				cont = false;
				break;
			case '\n':	//if newline is hit
				stop = true;
				cont = false;
				break;
			default:
				i++;
			}
		}

		if (i - begin > 0) {
			memcpy(token[tok].str, &line[begin], i - begin);
			token[tok].hastoken = true;
			tok++;
		}

		i++;

		if (stop || tok == 3) break;
	}
}

char* substr(char* string, int pos, int length)
{
	char sub[100];
	int c = 0;
	while (c < length) {
		sub[c] = string[pos+c-1];
		c++;
	}
	sub[c] = '\0';
	char *substring = sub;

	return substring;	
}

long toHex(char hstr)
{
	char *p;
	return strtol(&hstr, &p, 16);
}

void T_clear()
{
	for (int i = 0; i < 4; i++)
	{
		strcpy(token[i].str, "\0");
		token[i].hastoken = false;
	}
}

void S_clear()
{
	for (int i = 0; i < 500; i++)
	{
		memset(SYMTAB[i].label, ' ', 6);
	}
}

bool symSearch(char *label)
{
	bool found = false;

	//if SYMTAB is empty
	if (SYMTAB[0].label[0] == '\0' && SYMTAB[0].address == -2) return false;

	//loop through and check if label exists
	for (int i = 0; i < 500; i++){ 
		if (strcmp(label, SYMTAB[i].label) == 0){
			SYM = &SYMTAB[i];
			found = true;
			break;
		}
	}

	if (found) return true;
	else return false;
}

void symInsert(char *label, long addr)
{
	bool found = false;		//is label already in symbol table?
	bool operand = false;	//is label given in the operand field?
	char ltmp[7];			//tmp file to mod label and strcat if needed
	//char tab[3] = "\t";		//holds tab char to format short labels
	char space[3] = " ";	//holds space char to format short labels
	//int i;

	if (addr == -1) operand = true; //otherwise label is in label field

	if (!operand)	//if label is in label field
	{
		//check if it is in the symbol table
		found = symSearch(label);
		if (found){	//SYM points to the symbol in SYMTAB[] if found
			if (SYM->address == -1){	//if symbol is in symbol table but has no address
				SYM->address = addr;	//set address for label
				SYM = 0;	//reset pointer
			}
			else {	//error duplicate label ** Error 0
				fprintf(Errors, "0 ");
				ErrorCount++;
				SYM = 0;	//reset pointer
			}
		}
		else {	//SYM points to nothing, insert label into SYMTAB[] and increment symI
			strcpy(SYMTAB[symI].label, label);

			//if the label is shorter than 4 chars then format it for SymbolTable.txt
			for (int i = 0; i < 5; i++){
				if (SYMTAB[symI].label[i] == ' ') {
					strcpy(ltmp, label);
					strcat(ltmp, space);
					strcpy(SYMTAB[symI].label, ltmp);
				}
			}
			SYMTAB[symI].address = addr;
			symI++;
		}
	}
	else  // if label is in operand field
	{
		//if program is at this point then it has already been determined 
		//that the label is not in the symbol table yet. So insert it with
		//address set to -1 and increment symI
		strcpy(SYMTAB[symI].label, label);
		SYMTAB[symI].address = -1;
		symI++; //move to next index in SYMTAB[]	
	}	
}

void smartLoc()
{
	long operand;
	int oplen = strcspn(token[2].str, "\0");

	if (token[2].hastoken){		//is there an operand?
		operand = strtol(token[2].str, NULL, 10);
	}
    else if (!token[0].hastoken) {
        locctr += 3;	//increment locctr when RSUB
        return;
    }
    else {  //invalid empty operand ** Error 2
        fprintf(Errors, "2 ");
        ErrorCount++;
        return;
    }

	if (strcmp(token[1].str, "START") == 0) {	//set start address
		locctr = operand;
	}
	else if (strcmp(token[1].str, "RESB") == 0){
		if (isdigit(atoi(token[2].str)) == 0){	//check if operand is int
			fprintf(Errors, "2 ");
			ErrorCount++;
		}
		else locctr += operand;
	}
	else if (strcmp(token[1].str, "RESW") == 0){
		if (isdigit(atoi(token[2].str)) == 0){	//check if operand is int
			fprintf(Errors, "2 ");
			ErrorCount++;
		}
		else locctr += (3 * operand);
	}
	else if (strcmp(token[1].str, "WORD") == 0){
		if (isdigit(atoi(token[2].str)) == 0){	//check if operand is int
			fprintf(Errors, "2 ");
			ErrorCount++;
		}
		else locctr += 3;
	}
	else if (strcmp(token[1].str, "BYTE") == 0){
		int digits = oplen - 3;

		//check for single quotes (i.e. X'4F') 
		if (token[2].str[1] != '\'' || token[2].str[oplen - 1] != '\''){
			fprintf(Errors, "2 ");
			ErrorCount++;
			return;
		}
		if (token[2].str[0] == 'C') locctr += digits;
		if (token[2].str[0] == 'X'){
			if (digits % 2 == 1) {	//error: odd number of digits when using X ** Error 2
				fprintf(Errors, "2 ");
				ErrorCount++;
			 	return;
			 }	
			else locctr += digits/2;
		}
		else {	//invalid operand ** Error 2
			fprintf(Errors, "2 ");
			ErrorCount++;
		}	
	}
	else if (strcmp(token[1].str, "END") == 0){
		progLen = locctr - progStart;
	}
	else locctr += 3;

}

void indexOp(char *Operand)
{
	Operand = token[2].str;
	int oplen = strcspn(Operand, "\0");
	int comma = strcspn(Operand, ",");
	if (Operand[oplen - 2] == ','){
		if (Operand[comma] != 'X'){	//invalid operand if not X ??? does it have to be X???
			fprintf(Errors, "2 ");
			ErrorCount++; 
		}
	}
	else return;
}

void Pass1()
{
	bool begin = false;				//has start been called?
	bool stop = false;				//has stop of source file been
	//int length;						//length of line
	bool tok1, tok2, tok3, tok4;	//does token exist?
	//char label[6];					//stores label to input as parameter
	char ErrorLine[100];			//stores Error line in Errors to print to intermediate file
	char ErrFile[20] = "Error.tmp";

	//clear SymbolTable and 'line[]'
	S_clear();
	memset(line, '\0', 500);

	printf("Beginning Pass 1...\n\n");	//status print

	while(!feof(source) && !begin)
	{	
		//open tmp file to push errors to
		Errors = fopen(ErrFile, "w");
		fprintf(Errors, "Errors: ");

		fgets(line, 500, source);	//read in source line

		//Test print
		//printf("Line: %s\n\n",line);

		Tokenize(line);		//get tokens

		//intitalize tok booleans
		tok1 = token[0].hastoken;
		tok2 = token[1].hastoken;
		tok3 = token[2].hastoken;
		tok4 = token[3].hastoken;

		smartLoc();	//get starting address

		if (line[0] == '.')	//if comment only
		{
			if (token[3].str[0] != '.') fprintf(intermediate, ".%s\n\n", token[3].str);
			else fprintf(intermediate, "%s\n\n", token[3].str);
			continue;	//move on to next line in source file
		}
		else 	//if not a comment process line
		{
			begin = true;		//start error checking

			if (tok1) {
				symInsert(token[0].str, locctr);
			}
			if (!tok2){	//missing operation ** Error 2
				fprintf(Errors, "2 ");
				ErrorCount++; 
			}
			if (strcmp(token[1].str, "START") == 1){ //invalid operation ** Error 2
				fprintf(Errors, "2 ");
				ErrorCount++;
			}
			else if (tok2 && strcmp(token[1].str, "START") == 0)
			{
				if(!tok3){ //missing starting address ** Error 4
					fprintf(Errors, "4 ");
					ErrorCount++;
				} 
			}
			if (!tok1 && !tok3 && tok2)	//RSUB
			{
				//if this combination exists then the directive should be RSUB
				if (strcmp(token[1].str, "RSUB") == 1){ //invalid operation ** Error 1
					fprintf(Errors, "1 ");
					ErrorCount++;
				}
			}
			if (strcmp(token[1].str, "RSUB") == 0 && tok3){ //invalid operation ** Error 1
				fprintf(Errors, "1 ");
				ErrorCount++;
			}
		}

		//print to intermediate file
		fprintf(intermediate, "Source line: %s\n", line);
		fprintf(intermediate, "Location counter: %ld\n", locctr);
		fprintf(intermediate, "Label: %s\n", token[0].str);
		fprintf(intermediate, "Operation: %s\n", token[1].str);
		fprintf(intermediate, "Operand: %s\n", token[2].str);
		//fprintf(intermediate, "Errors: (NOT READY)\n\n");
	  	fclose(Errors);
		Errors = fopen(ErrFile, "r");
		fgets(ErrorLine, 100, Errors);
		fclose(Errors);
		Errors = fopen(ErrFile, "w");
		fprintf(intermediate, "%s\n\n", ErrorLine);
		if (strlen(ErrorLine) < 9) fprintf(intermediate, "Errors: None\n\n");
		else fprintf(intermediate, "\n\n");
		fprintf(Errors, "\n");
    	memset(ErrorLine, '\0', 100);
	}

	while (!feof(source) && !stop)
	{
		//open tmp file to push errors to
		fprintf(Errors, "Errors: ");

		fgets(line, 500, source);	//read in source line

		//Test print
		//printf("Line: %s\n\n",line);

		Tokenize(line);		//get tokens

		//intitalize tok booleans
		tok1 = token[0].hastoken;
		tok2 = token[1].hastoken;
		tok3 = token[2].hastoken;
		tok4 = token[3].hastoken;

		if (line[0] == '.')	//if comment only
		{
			if (token[3].str[0] != '.') fprintf(intermediate, ".%s\n\n", token[3].str);
			else fprintf(intermediate, "%s\n\n", token[3].str);
			continue;	//move on to next line in source file
		}
		else 	//if not a comment process line
		{
			if (tok1) symInsert(token[0].str, locctr);
			
			//

			if (!tok1 && !tok3 && tok2)	//RSUB
			{
				//if this combination exists then the directive should be RSUB
				if (strcmp(token[1].str, "RSUB") == 1){ //invalid operation ** Error 1
					fprintf(Errors, "1 ");
					ErrorCount++;
				}
			}

		}

		smartLoc();

		if (locctr > 32768){	//program too long ** Error
			fprintf (Errors, "7 ");
			ErrorCount++;
			break;
		}

		//print to intermediate file
		fprintf(intermediate, "Source line: %s\n", line);
		fprintf(intermediate, "Location counter: %ld\n", locctr);
		fprintf(intermediate, "Label: %s\n", token[0].str);
		fprintf(intermediate, "Operation: %s\n", token[1].str);
		fprintf(intermediate, "Operand: %s\n", token[2].str);
		//fprintf(intermediate, "Errors: (NOT READY)\n\n");
	  	fclose(Errors);
		Errors = fopen(ErrFile, "r");
		fclose(Errors);
		Errors = fopen(ErrFile, "w");
		fgets(ErrorLine, 100, Errors);
		fprintf(intermediate, "%s", ErrorLine);
		if (strlen(ErrorLine) < 9) fprintf(intermediate, "Errors: None\n\n");
		else fprintf(intermediate, "\n\n");
		fprintf(Errors, "\n");
    	memset(ErrorLine, '\0', 100);

		if (strcmp(token[1].str, "END") == 0) break;
	}

	if (strcmp(token[1].str, "END") != 0){		//invalid or missing END directve ** Error 5
		fprintf(Errors, "5 ");
		ErrorCount++;
	}

	//print to symbol table
	fprintf(symboltable, "Label			Address\n\n");
	for (int i = 0; i < (symI); i++){
		fprintf(symboltable, "%s			%ld\n", SYMTAB[i].label, SYMTAB[i].address);
	}

	if (locctr > 32768){	//status print
		printf("Unable to finish Pass 1 (Max program length has been reached)\n");
	}
	else {
		//status print
		printf("Pass 1 Complete!\n");
	}

	//close files 
	fclose(source);
	fclose(intermediate);
	fclose(symboltable);
	fclose(Errors);
	remove(ErrFile);
}


