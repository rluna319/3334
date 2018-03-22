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

	/*void Error0(){printf("%s\n", Error[0].output);}
	void Error1(){printf("%s\n", Error[1].output);}
	void Error2(){printf("%s\n", Error[2].output);}
	void Error3(){printf("%s\n", Error[3].output);}
	void Error4(){printf("%s\n", Error[4].output);}					I probably don't need these... -_-
	void Error5(){printf("%s\n", Error[5].output);}
	void Error6(){printf("%s\n", Error[6].output);}
	void Error7(){printf("%s\n", Error[7].output);}
	void Error8(){printf("%s\n", Error[8].output);}
	void Error9(){printf("%s\n", Error[9].output);}
	//void Error10(){printf("%s\n", Error[10].output);}*/

//						--End ErrorFlags--

//-----------------------File Declarations--------------------//

FILE *source, *intermediate, *symboltable, *Errors;


//-----------------------Program Functions--------------------//  

//splits char array (string)
void split (char *, char *, char *, char *, int *);
//Taks a string and parses it into tokens
void Tokenize(char *);
//Clears token[]
void T_clear();
//Prints to file
void T_fprint();
//Pass 1 (getting errors and writing to the 'symboltable' and 'intermediate' files)
void Pass1();
//get line if file being read
void getLine(char *);
//for easy oneliner conversion from string to hex
long toHex(char);
//insert label into SYMTAB
void symInsert(char *, long);
//Clears SYMTAB[]
void S_clear();
//search funtion for symbol table
bool symSearch(char *);


//-----------------------Global Variables---------------------//

long LC = 0; 		//LC counter
int begin = 0;		//program begin LC
int progLen = 0;	//legnth of program
int comment = 0;	//program comment LC
char progName[20];	//name of program
char line[500];		//stores line in file
int tcount = 0;		//token counter
int symI = 0;		//SYMTAB indexer/counter
//int symC = 0;		//symbol counter
int ErrorCount = 0;	//Error count
char Sname[20];		//stores source file name


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
	//Open source file for reading
	strcpy(Sname, prm1);
	source = fopen(prm1, "r");
	if (source == NULL)		//file not located (stop loadf)
	{
		printf ("%s does not exist.\nPlease try again.", prm1);
		printf ("Remember that filenames are case-sensitive...\n\n");
		return;
	}
	else printf ("%s successfully loaded\n", prm1);	//file located

	//Open intermediate and symbol table files for writing
	intermediate = fopen("Intermediate.txt", "w");
	symboltable = fopen("SymbolTable.txt", "w");
	if (intermediate == NULL) {printf("%s\n\n", Error[8].output);}	//unable to open intermediate file for writing 
	else printf ("\n'Intermediate.txt' file created...\n");
	if (symboltable == NULL) {printf("%s\n\n", Error[9].output);}	//unable to open symboltable for writing 
	else printf ("'SymbolTable.txt' file created...\n");

	//initialize OpTable and line array
	OpTable();
	strcpy(line, "\0");

	//neg1 print
	fprintf(intermediate, "Intermediate File\n");
	fprintf(intermediate, "--Contatins: source line, LC counter, mnemonics, operands, and errors--\n\n");
	return;
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
	printf ("   ~ assemble - assembles the source file loaded . \n"); 
	printf ("----------------------------------------------------------------------------------------------------------\n");
}

void assemble(char *prm1)
{
	if (prm1 == Sname)
	{
		if (source != NULL && intermediate != NULL && symboltable != NULL) Pass1();
		else {
			printf("Error:\n Source program not loaded and/or 'Intermediate.txt' and/or 'SymbolTable.txt not created...\n");
			printf("-Remember you must load the source file prior to calling assemble-\n\n");
		}
	}
	else {
		printf("	Error!\n	File not found. Filenames are case sensitive!\n\n");
		printf("	-Remember you must load the source file prior to calling assemble-\n\n");
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


	//				end Pass 1 Errors
}

void Tokenize(char *line)
{
	bool cont;		//continue loop?
	bool blank;		//blank token?
	bool end;		//early end of line?
	int tok = 0; 	//index var for 'token[]'
	int i = 0;		//index var for 'line[]'

	T_clear();
	cont = blank = true;
	end = false;

	//if line is a comment do not tokenize
	if (line[0] == '.') 
	{
		strcpy(token[3].str, line);
		return;
	}

	//tokenize loop
	for (int i = 0; tok < 4; i++)
	{
		cont = true;
		blank = true;
		int begin = i;

		while(cont)
		{
			switch(line[i])
			{
				case ' ':	//if space is hit
					i++;
					if (tok == 3) continue;
					else cont = false;
				case '\t':	//if tab is hit
					i++;
					if (tok == 3) continue;
					else cont = false;
				case '\r':
				case '\v':
				case '\0':	//if empty char is hit
					if (tok < 3) end = true;
					cont = false;
					break;
				case '\n':	//if newline is hit
					if (tok < 3) end = true;
					cont = false;
					break;
				default:
					blank = false;
					i++;
			}
		}

		if(!blank)
		{
			memcpy(token[tok].str, &line[begin], (i - 1) - begin);
			token[tok].str[i - begin + 1] = '\0';
			tcount++;
			token[tok].hastoken = true;
		}

		tok++;

		if (end) break;
	}
}

void T_clear()
{
	for (int i = 0; i < 4; i++)
	{
		strcpy(token[i].str, "\0");
		token[i].hastoken = false;
	}
}

void T_fprint()	//?? do i need this?? **************
{

}

long toHex(char hstr)
{
	char *p;
	return strtol(&hstr, &p, 16);
}

void S_clear()
{
	for (int i = 0; i < 500; i++)
	{
		memset(SYMTAB[i].label, '\0', 6);
	}
}

bool symSearch(char *label)
{
	bool found = false;
	for (int i = 0; i < 500; i++){ 
		if (strcmp(label, SYMTAB[i].label)){
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
	bool neg1 = false;		//is label address = -1 ? //?? do i need this?? ********
	int i;

	if (addr == -1) operand = true; //otherwise label is in label field

	if (!operand)	//if label is in label field
	{
		//check if it is in the symbol table
		found = symSearch(label);
		if (found){	//SYM points to the symbol in SYMTAB[] if found
			if (SYM->address == -1){	//if symbol is in symbol table but has no address
				//neg1 = true;			***********
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
			SYM->address = addr;
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

void Pass1()
{
	bool begin = false;				//has start been called?
	bool end = false;				//has end of source file been
	int length;						//length of line
	bool tok1, tok2, tok3, tok4;	//does token exist?
	char label[6];					//stores label to input as parameter
	char ErrorLine[100];			//stores Error line in Errors to print to intermediate file
	bool symERR;					//was there an error? //?? do i need this?? ***************

	//set location counter to 0
	LC = 0;	

	//open tmp file to push errors to
	char ErrFile[20] = "Error.tmp";
	Errors = fopen(ErrFile, "w");
	if (Errors == NULL) printf("!!Unable to open 'Error.tmp' for writing. *LINE: %d\n\n", __LINE__);
	fprintf(Errors, "Error: ");

	//clear SymbolTable and 'line[]'
	S_clear();
	memset(line, '\0', 500);

	printf("Beginning Pass 1...");	//status print

	while(!feof(source) && !begin)
	{
		fgets(line, 500, source);	//read in line
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
			begin = true;		//start error checking
			if (tok1) symInsert(token[0].str, LC);
			if (!tok2){	//missing start directive ** Error 2
				fprintf(Errors, "2 ");
				ErrorCount++; 
			}
			if (tok2 && !strcmp(token[1].str, "START")){ //invalid operation ** Error 2
				fprintf(Errors, "2 ");
				ErrorCount++;
			}
			else if (tok2 && strcmp(token[1].str, "START"))
			{
				if(!tok3){ //missing starting address ** Error 4
					fprintf(Errors, "4 ");
					ErrorCount++;
				} 
				else LC = toHex(*token[2].str);	//set location counter to starting address
			}
			if (!tok1 && !tok3 && tok2)	//RSUB
			{
				//if this combination exists then the directive should be RSUB
				if (!strcmp(token[1].str, "RSUB")){ //invalid operation ** Error 1
					fprintf(Errors, "1 ");
					ErrorCount++;
				}
			}
		}

		//print to intermediate file
		fprintf(source, "Source line: %s\n", line);
		fprintf(source, "Location counter: %ld\n", LC);
		fprintf(source, "Label: %s\n", token[0].str);
		fprintf(source, "Operation: %s\n", token[1].str);
		fprintf(source, "Operand: %s\n", token[2].str);
		fgets(ErrorLine, 100, Errors);
		fprintf(source, "%s\n\n", ErrorLine);
		fprintf(Errors, "\n");
	}
	while (!feof(source) && !end)
	{

	}

	//print to symbol table
	fprintf(symboltable, "Label			Address\n\n");
	for (int i = 0; i < (symI + 1); i++){
		fprintf(symboltable, "%s			%ld\n", SYMTAB[i].label, SYMTAB[i].address);
	}

	//close files and delete 'Error.tmp'
	fclose(source);
	fclose(intermediate);
	fclose(symboltable);
	remove(ErrFile);

	//double check that the temporary Error file was removed succesfully
	if (Errors != NULL) printf("!Error.tmp was not successfully removed! *Line: %d\n\n", __LINE__);
}

//**************************** UNFINISHED! :( ***************************//