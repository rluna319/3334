
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <limits>
#include <cstdio>
#include <fstream>
#include <stdint.h>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "sicengine.c"

using namespace std;

//for instructions and directives
class instType {
  public:
    string instruction;
    string opcode;
};

//for labels (symbols)
class symType {
  public:
    string label;
    int address;
    symType():label(""),address(-1){ }
};

class asmType {

  //hash tables are not resized
  //OPTAB has a perfect minimal hash (uses r())
  //SYMTAB uses linear probing with step of 7 (coprime to 1009)
  vector<instType> OPTAB;	//mnemonic and opcode value
  vector<symType> SYMTAB;	//case-insensitive labels and addresses
  vector<int> r;

  int symCount;		//# of symbols in SYMTAB, can't exceed 500
  const int symMax;

    int locctr;
    int start;
    int progLen;
    int end;
    string progName;
    string instruction;
    string label;
    string operand;
    vector<string> srcTokens;
    string srcLine;
    vector<int> errflags;
    bool endSet;
    string endOperand;
    int firstExec;
    bool hasError;
    bool pass1Error;
    string loadAddr;

  string fname;	// name of the assembly source file

  void initOptab();
  //Store instructions and relevant opcode values in OPTAB

  int processLine();
  //Processes a source line (checks for errors, updates LOCCTR, stores symbols)
  //Returns 0 if blank or a comment, else returns 1

  int showSymtab();
  //Print the symbol table to symbtab.txt (label, address, index)
  //Returns 1 if an address is not assigned to a label, else returns 0

  int instructionHash(string key);
  //Provides hash values for instructions (mnemonics)

  int symbolHash(string key);
  //Provides hash values for labels (symbols)

  int symtabInsert(string key, int value);
  //Store label and its address (if available) in symbol table
  // if address is already assigned, error

  int getSymAddr(string key);
  //Retrieve the address of a label (symbol) from the SYMTAB
  //Returns an integer address >= 0 if symbol has been assigned one, else -1

  bool isLabel(string label);
  //Returns true if the string is a valid label, else false
  //  labels are alphanumeric, start with letter, length <= 6, not instructions

  string getInstruction(string mnemonic);
  //Returns opcode of mnemonic (instruction or directive) if it exists, else ""
  //  directives return their names

  bool isHex(string hexstr);
  //Returns true if the hexadecimal string is valid, else false
  //  string cannot: be more than 32 hex digits, have odd # of digits,
  //    start with a letter
  //(note: keep as string since (2^128)-1 may result in overflow)

  bool isAddr(string addr);
  //Like isHex, but ignores the 32 hex digit limit and even digit requirement
  //Could merge the two. . .

  public:

	asmType(string filename):fname(filename), symMax(500), symCount(0) {
		SYMTAB.clear();
		SYMTAB.resize(1009);
		OPTAB.clear();
		OPTAB.resize(31);
		r.clear();
	};

	void pass1();
	//First pass of the assembler
    // Check for any errors, assign addresses to all symbols, translate
    //  instructions to opcode values

    void pass2();
    //Second pass of the assembler
    // Create a listing file and (if no errors in pass 1) object code

	string getFName()
	{	return fname;	};

};

/*   ---Functions---   */

void welcome();
//Emits welcome text

void commandLoop();
//Loop emits prompt, accepts commands, calls commandEval() to process them

bool commandEval(vector<string> &cmdTokens);
//Referenced vector may contain 0 or more strings
//Calls the appropriate command function (if recognized)
//Returns false if user inputs 'exit' command (to end loop, i.e. program),
//  Else returns true to continue

void noCommand();
//Emits error text for when a command is not recognized

void badParam(string cmd, int numParam);
//Emits error text for when incorrect # of parameters are used with a command

void help();
//Emits command list

void directory();
//Lists files stored in the current directory

void load(string fname);
//Loads an object program into memory using the SIC simulator

void execute();
//Uses the SIC simulator to execute a loaded program

void debugHelp();
//Prints the commands available in debug mode

void debug();
//This is a stub.

void dump(string start, string end, bool noVals);
//This is a stub. The command requires two parameters.
//If BOTH parameters aren't given (noVals = true), user is prompted for them.

void assemble(string fname);
//A two-pass assembler tries to create object code for a source program

void exit();
//Emits exit text

///// HELPFUL FUNCTIONS /////

void tokenize(const char *str, vector<string> &tokens);
//This breaks apart a string by delimiters and stores pieces in a vector.
//The vector remains empty if no tokens (non-whitespace characters) are found.
//The delimiters include most ASCII whitespaces. Extra whitespace is ignored.

void tokenizeStr(string str, vector<string> &tokens);
//Like tokenize() but takes a string variable

void tokendump(const vector<string> &V);
//Emits # of elements in V, then its elements (for testing purposes)

string toUpper(const string &s);
//Returns uppercase string


int main()
{
  SICInit();
  welcome();
  commandLoop();
  return 0;
}


/*   ---Functions---   */

void welcome()
{
  cout<<"\nWelcome to the SIC Simulator.\n";
  cout<<"=============================\n";
  cout<<"Enter a command after the prompt.\n";
  cout<<"Enter HELP for a list of available commands.\n";
} //end welcome()

void commandLoop()
{
  char cmdStr[256];	  //stores line entered into prompt
  bool contLoop = true;	  //determines if loop continues
  vector<string> tokens;  //holds tokenized string from user input

  //Each iteration empties the vector, receives and tokenizes a line of input,
  //  and processes the command (if any) until the EXIT command is entered
  while(contLoop)
  {
	tokens.clear();
	cout<<"\nCommand>";
	cin.getline(cmdStr, 256);
	tokenize(cmdStr, tokens);
  	contLoop = commandEval(tokens);
  }

} //end commandLoop()

bool commandEval(vector<string> &cmdTokens)
{

  if( cmdTokens.empty() )
  {
	noCommand();
	return true;
  }
  
  string cmd = toUpper(cmdTokens[0]);	// 1st token is a command
  int cmdLen = cmd.length();
  int numParams = cmdTokens.size() - 1; // paramaters after the command

  if(cmdLen > 2)  {
    if( "EXI" == cmd.substr(0,3) ) {
	if(numParams > 0) {
	  badParam("EXIT", 0);
	  return true;
	}
	else {
	  exit();
	  return false;
	}
    } // EXIT
    else if( "EXE" == cmd.substr(0,3) ) {
	if(numParams > 0) {
	  badParam("EXECUTE", 0);
	  return true;
	}
	else {
	  execute();
	  return true;
	}  
    } // EXECUTE
  } // end checks for EXIT, EXECUTE commands

  if(cmdLen > 1) {
    if( "DE" == cmd.substr(0,2) ) {
	if(numParams == 0)
	  debug();
	else
	  badParam("DEBUG", 0);
	return true;	
    } // DEBUG

    if( "DI" == cmd.substr(0,2) ) {
	if(numParams == 0)
	  directory();
	else
	  badParam("DIRECTORY", 0);
	return true;
    } // DIRECTORY

    if( "DU" == cmd.substr(0,2) ) {
	if(numParams == 2)
	  dump(cmdTokens[1], cmdTokens[2], 0);
	else if(numParams == 0)
	  dump("", "", 1);
	else
	  badParam("DUMP", 2);
	return true;	
    } // DUMP

  } // end checks for DEBUG, DIRECTORY, DUMP commands

  if(cmd[0] == 'H') {
    help();
    return true;
  } // check for HELP command

  if(cmd[0] == 'A') {
    if(numParams == 1)
	assemble(cmdTokens[1]);
    else
	badParam("ASSEMBLE", 1);
    return true;
  } // check for ASSEMBLE command

  if(cmd[0] == 'L') {
    if(numParams == 1)
	load(cmdTokens[1]);
    else
	badParam("LOAD", 1);
    return true;
  } // check for LOAD command

  noCommand(); // no matching command

  return true;

} //end commandEval()

void noCommand()
{
  cout<<"\n  Command not recognized.";
  cout<<"\n  Enter HELP for a list of available commands.\n";
}

void badParam(string cmd, int numParam)
{
  cout<<"\n  Incorrect number of parameters.\n  ";
  cout<<cmd<<" requires "<<numParam<<" parameter(s).\n";
}

void help()
{
  cout<<"\n  Commands are recognized by their first unique character(s)";
  cout<<"\n  Filenames are case-sensitive!\n";
  cout<<"\n  Command____Parameters______Purpose";
  cout<<"\n  LOAD       [filename]      Loads a file";
  cout<<"\n  ASSEMBLE   [filename]      Assembles and stores a SIC program";
  cout<<"\n  DUMP       [start] [end]   Dumps stuff";
  cout<<"\n  EXECUTE    NONE            Executes program loaded in memory";
  cout<<"\n  DEBUG      NONE            Executes in debug mode";
  cout<<"\n  DIRECTORY  NONE            Lists files in the current directory";
  cout<<"\n  HELP       NONE            Helps you help yourself";
  cout<<"\n  EXIT       NONE            Exits this simulator\n";
}

void directory()
{
  cout<<"\n";
  system("ls");
} //end directory()

void load(string fname)
{
  // open file, PutMem

  ifstream inf(fname.c_str());

  if(!inf.good()) {
    cout<<"\n  No such file exists: "<<fname;
    cout<<"\n  Filenames are case-sensitive. . .\n";
    return;
  }

  string srcLine;
  getline(inf, srcLine);    // grab object file header

  if(srcLine.length() < 19) {
    cout << "\n  There is an issue with the object file header.\n";
    return;
  }
  
    // unsigned longs used instead of ADDRESS
  unsigned long startAddr = strtol(srcLine.substr(7, 6).c_str(), NULL, 16);
  unsigned long progLen = strtol(srcLine.substr(13,6).c_str(), NULL, 16);
  unsigned long loadAddr = 0;
  int lineLength = 0;

  BYTE memoryContents;  // 8 bits, max hex # = FF
  
  while(getline(inf, srcLine)) {
    if(srcLine.empty()) {
        cout << "\n  There is an issue with the object file text record.\n";
        return;
    }
    if(srcLine[0] == 'E') {
        if(srcLine.length() != 7) {
            cout << "\n  There is an issue with the end record.\n";
            return;
        }

        // set PC to first instruction to be executed
        PutPC(strtol(srcLine.substr(1,6).c_str(), NULL, 16));
        break;
    }
    if(srcLine[0] != 'T' || srcLine.length() < 19) {
        cout << "\n  There is an issue with the object file text record.\n";
        return;
    }

    loadAddr = strtol(srcLine.substr(1,6).c_str(), NULL, 16);
    lineLength = strtol(srcLine.substr(7,2).c_str(), NULL, 16) * 2;

    for(int j = 0; j != lineLength; j+=2) {
        memoryContents = strtol(srcLine.substr(9+j,2).c_str(), NULL, 16);
        PutMem(loadAddr, &memoryContents, 0);
        ++loadAddr;
    }

  }

  cout<<"\n  "<<fname<<" has been loaded. . .\n";
} //end load()

void execute()
{
  ADDRESS execAddr, tempPC;
  
  execAddr = tempPC = GetPC();
  SICRun(&execAddr, 0);

  // reset the PC to the original address for execution start
  PutPC(tempPC);

  cout<<"\n  Execution is complete, but did it really work?\n";
} //end execute()

void debugHelp()
{
  cout << "\n  Commands [Enter the single character in brackets]";
  cout << "\n  [p] - Print contents of all registers";
  cout << "\n  [c] - Change contents of register [X, L, A] or memory";
  cout << "\n  [e] - Execute the next (single) step";
  cout << "\n  [s] - Stop debugging and complete program execution";
  cout << "\n  [q] - Quit debug mode and halt execution\n";
}

void debug()
{
  char selection = 'r', chgSel, ccChar;
  string hexStr;
  ADDRESS execAddr, tempPC, regPC, memLoc;
  WORD regs[6];
  WORD regA, regX, regL, regSW, chgWord;
  BYTE chgByte;

  execAddr = tempPC = GetPC();

  cout << "\n  Debug Mode!\n";
  debugHelp();

  // NOTE: 'q' will reset the PC
  while(selection != 'q') {

	cout << "\ndbg>";
	cin >> selection;

	selection = tolower(selection);

	switch( selection ) {
		case 'h': debugHelp();

		case 'p':

            // print out registers
            GetReg(regs); // A X L B S T  
            cout << "\n  A: ";
            for(int j = 0; j < 3; ++j)
                cout << hex << uppercase <<
                    setw(2) << setfill('0') << (unsigned long) (regs[0][j]);

            cout << "    X: ";
            for(int j = 0; j < 3; ++j)
                cout << hex << uppercase <<
                    setw(2) << setfill('0') << (unsigned long) (regs[1][j]);

            cout << "    L: ";
            for(int j = 0; j < 3; ++j)
                cout << hex << uppercase <<
                    setw(2) << setfill('0') << (unsigned long) (regs[2][j]);
            
            cout << "    PC: " << hex << uppercase << (unsigned long) (GetPC());

		ccChar = GetCC();
            cout << "    SW: " << ccChar << "\n";
            			
			break;

		case 'c':

            // change value of a register or contents at a memory location
            cout << "\n  Change the value of [A], [X], [L], or [M]emory>";
            cin >> chgSel;

            chgSel = tolower(chgSel);
		if(chgSel != 'a' && chgSel != 'x'&& chgSel != 'l'
			&& chgSel != 'm') {
			cout << "\n  Command does not match any letters "
				<< "in brackets (e.g. [A]).\n";
		}
		else {

			GetReg(regs); // A X L, 3 bytes each
			cout << "  Enter 6 hex digits for a register, else "
			<< "the memory location in hex. >";
			cin >> hexStr;

            switch( chgSel ) {
                case 'a':	// A register
			if(hexStr.length() == 6) {
				for(int i = 0; i != 3; ++i) {
		        regs[0][i] = strtol(hexStr.substr(i*2, 2).c_str(), NULL, 16);
				}
				PutReg(regs);
			}
			else
				cout << "  Incorrect input.\n";
                    break;
                
                case 'x':	// X register
			if(hexStr.length() == 6) {
				for(int i = 0; i != 3; ++i) {
		        regs[1][i] = strtol(hexStr.substr(i*2, 2).c_str(), NULL, 16);
				}
				PutReg(regs);
			}
			else
				cout << "  Incorrect input.\n";
                    break;

                case 'l':	// L register
			if(hexStr.length() == 6) {
				for(int i = 0; i != 3; ++i) {
		        regs[2][i] = strtol(hexStr.substr(i*2, 2).c_str(), NULL, 16);
				}
				PutReg(regs);
			}
			else
				cout << "  Incorrect input.\n";
                    break;

                case 'm':	// Memory location
			memLoc = strtol(hexStr.c_str(), NULL, 16);
			cout << "  Enter the new contents.\n  Use 2 hex digits "
				<< "for a byte, 6 hex digits for a word. >";
			cin >> hexStr;
	
			if(hexStr.length() == 6) {
				for(int i = 0; i != 3; ++i) {
		        chgWord[i] = strtol(hexStr.substr(i*2, 2).c_str(), NULL, 16);
				}
				PutMem(memLoc, chgWord, 1);
			}
			else if(hexStr.length() == 2) {
		        chgByte = strtol(hexStr.c_str(), NULL, 16);
				PutMem(memLoc, &chgByte, 0);
			}
			else
				cout << "  Incorrect input.\n";
                    break;
		default: cout << "\n  Command not recognized.\n";
            }	// end switch
		} // end else

			break;

		case 'e': SICRun(&execAddr, 1); // execute a single step

			break;

		case 's': SICRun(&execAddr, 0); // execute until program completion
			  selection = 'q';

			break;

		case 'q': 

			break;

		default:
		cout << "\n  Command not recognized. Enter 'h' for help.\n";
			break;
	}

  }

  PutPC(tempPC);

  cout << "\n  End Debug Mode!\n";

    cin.clear();
    int imax = std::numeric_limits<int>::max();
    cin.ignore(imax,'\n');

} //end debug()

void dump(string start, string end, bool noVals)
{
  if(noVals)
  {
    cout<<"\n  Enter the starting hex value:";
    cin>>start;
    cout<<"  Enter the ending hex value:";
    cin>>end;

    //input above interferes with getline in commandLoop()
    //this clears the buffer of '\n' etc.
    cin.clear();
    int imax = std::numeric_limits<int>::max();
    cin.ignore(imax,'\n');
  }

  BYTE value;
  ADDRESS startAddr, endAddr, temp;

  startAddr = strtol(start.c_str(), NULL, 16);
  endAddr = strtol(end.c_str(), NULL, 16);

  cout << "\n  Any invalid hex values will be set to 0\n";
  cout << "\n  Dumping contents from " << hex << startAddr << " to "
 	<< endAddr << "\n";
 
  if(startAddr > endAddr) {
	temp = endAddr;
	endAddr = startAddr;
	startAddr = temp;
  }

  int pauseScreen = 0 ;
  while(startAddr <= endAddr) {
    ++pauseScreen;
    if(pauseScreen % 20 == 0) {
        cout << "\tPress ENTER to continue. . .";

        int imax = std::numeric_limits<int>::max();
        cin.clear();
        cin.ignore(imax,'\n');
        cout << "\n";
    }

	cout << startAddr << ": ";
	for(int i = 0; i < 16; ++i) {
		GetMem(startAddr+i, &value, 0);
		cout << uppercase << hex << setw(2) << setfill('0')
            << (int)value << " ";
	}
	startAddr += 16;
	cout << "\n";
  }

} //end dump()

void assemble(string fname)
{
  asmType aProgram(fname);
  aProgram.pass1();
  aProgram.pass2();
} //end assemble()

void exit()
{
  cout<<"\n  Goodbye.\n\n";
} //end exit()

///// asmType FUNCTIONS /////

void asmType::initOptab()
{
    r.push_back(2);
	r.push_back(17);
	r.push_back(1);
	r.push_back(1);
	r.push_back(0);
	r.push_back(1);
	r.push_back(16);	
	r.push_back(-8);
	r.push_back(5);
	r.push_back(0);	
	r.push_back(-1);
	r.push_back(18);
	r.push_back(0);
	r.push_back(0);	
	r.push_back(0);	
	r.push_back(20);	
	r.push_back(0);
	r.push_back(18);	
	r.push_back(23);
	r.push_back(20);
	r.push_back(3);
	r.push_back(11);	
	//r = {2,17,1,1,0,1,16,-8,5,0,-1,18,0,0,0,20,0,18,23,20,3,11};


	OPTAB[7].instruction = "ADD";
	OPTAB[7].opcode = "18";

	OPTAB[29].instruction = "AND";
	OPTAB[29].opcode = "58";

	OPTAB[14].instruction = "COMP";
	OPTAB[14].opcode = "28";

	OPTAB[27].instruction = "DIV";
	OPTAB[27].opcode = "24";

	OPTAB[17].instruction = "J";
	OPTAB[17].opcode = "3C";

	OPTAB[24].instruction = "JEQ";
	OPTAB[24].opcode = "30";

	OPTAB[2].instruction = "JGT";
	OPTAB[2].opcode = "34";

	OPTAB[3].instruction = "JLT";
	OPTAB[3].opcode = "38";

	OPTAB[16].instruction = "JSUB";
	OPTAB[16].opcode = "48";

	OPTAB[8].instruction = "LDA";
	OPTAB[8].opcode = "00";

	OPTAB[20].instruction = "LDCH";
	OPTAB[20].opcode = "50";

	OPTAB[18].instruction = "LDL";
	OPTAB[18].opcode = "08";

	OPTAB[10].instruction = "LDX";
	OPTAB[10].opcode = "04";

	OPTAB[0].instruction = "MUL";
	OPTAB[0].opcode = "20";

	OPTAB[22].instruction = "OR";
	OPTAB[22].opcode = "44";

	OPTAB[9].instruction = "RD";
	OPTAB[9].opcode = "D8";

	OPTAB[23].instruction = "RSUB";
	OPTAB[23].opcode = "4C";

	OPTAB[5].instruction = "STA";
	OPTAB[5].opcode = "0C";

	OPTAB[11].instruction = "STCH";
	OPTAB[11].opcode = "54";

	OPTAB[15].instruction = "STL";
	OPTAB[15].opcode = "14";

	OPTAB[6].instruction = "STX";
	OPTAB[6].opcode = "10";

	OPTAB[19].instruction = "SUB";
	OPTAB[19].opcode = "1C";

	OPTAB[13].instruction = "TD";
	OPTAB[13].opcode = "E0";

	OPTAB[12].instruction = "TIX";
	OPTAB[12].opcode = "2C";

	OPTAB[30].instruction = "WD";
	OPTAB[30].opcode = "DC";

	OPTAB[4].instruction = "START";
	OPTAB[4].opcode = "START";

	OPTAB[28].instruction = "END";
	OPTAB[28].opcode = "END";

	OPTAB[1].instruction = "BYTE";
	OPTAB[1].opcode = "BYTE";

	OPTAB[21].instruction = "WORD";
	OPTAB[21].opcode = "WORD";

	OPTAB[26].instruction = "RESB";
	OPTAB[26].opcode = "RESB";

	OPTAB[25].instruction = "RESW";
	OPTAB[25].opcode = "RESW";
} // end initOptab()

void asmType::pass1()
{

  ifstream inf(fname.c_str());

  if(!inf.good()) {
    cout<<"\n  No such file exists: "<<fname;
    cout<<"\n  Filenames are case-sensitive. . .\n";
    return;
  }

  cout<<"\n  Beginning pass one. . .\n";

  initOptab();

  fname = fname.substr(0,fname.find_first_of(".",0));

  //use the filename (sans any extension) for the intermediate file
  string outfstr = fname + ".int";

  ofstream outf(outfstr.c_str());

  locctr = 0;
  endSet = false;
  pass1Error = false;
  endOperand = "";
  label = "";
  instruction = "";
  operand = "";
  progName = "NONAME";
  start = 0;
  progLen = 0;
  firstExec = 0;
    
  int numTokens, s;
  int oldloc;
  bool startSet = false;  // if start location has been set
			  // if start is missing from first non-comment/blank
			  //   line, then set it (with addr 0)
  bool hasLabel = true;	  // if source line has a label
  bool goodLine;


  // process the first line(s) until start is set
  while(!inf.eof() && !startSet) {

    srcTokens.clear();
    errflags.clear();

    getline(inf, srcLine);
    tokenizeStr(srcLine, srcTokens);
    outf << srcLine << "\n!";
    numTokens = srcTokens.size();

    if( numTokens ) { // if line isn't blank, look at it

	if( srcLine[0] != '.' ) {   // line is not a comment, so translate it

	  startSet = true;
	  srcTokens[0] = toUpper(srcTokens[0]);

	  if( srcLine[0] == ' ' ) { 
    	    errflags.push_back(0); // missing start label
	    hasLabel = false;
	    instruction = srcTokens[0];	
	  }
	  else {
	    if(isLabel(srcTokens[0])) {
		label = srcTokens[0];
	    }
	    else
		errflags.push_back(7);	// illegal label

	    if( numTokens > 1 )
	    	instruction = toUpper(srcTokens[1]);	
	  }
	  
	  // if no START don't process line, ignore label for progName
	  if( instruction != "START" ) {

		s = symtabInsert(progName, locctr);
		errflags.push_back(1);
        pass1Error = false;
		for(int k=0; k!=errflags.size(); ++k)
		  outf << " " << errflags[k];		
		
		errflags.clear();

		if( s == 0 )		// duplicate label
			errflags.push_back(6);
		else if( s == -1 )	// too many symbols
			errflags.push_back(9);
		break;
	  }

 	  if( (numTokens - hasLabel) > 1 )
		operand = toUpper(srcTokens[(1 + hasLabel)]);
	  else {
		if(!label.empty())
			progName = label;
		s = symtabInsert(progName, locctr);
		if( s == 0 )		// dupe label
			errflags.push_back(6);
		else if ( s == -1 )	// too many symbols
			errflags.push_back(9);
		errflags.push_back(3);	// no start address
		errflags.push_back(8);	// missing operand
		break;
	  }

	  if( isHex(operand) )
		locctr = strtoul(operand.c_str(), NULL, 16);
	  else {
		errflags.push_back(8);	// missing/illegal operand
		errflags.push_back(4);	// invalid hex
	  }

	  if( locctr > 32768 )
		errflags.push_back(10);	// program too long
	  if( locctr < 0 ) {
		errflags.push_back(4);	// invalid hex
		locctr = 0;
	  }
	  start = locctr;
      progName = label;
	  s = symtabInsert(progName, locctr);
 

	  // neither of these should happen, but just in case. . .
	  if( s == 0 )		// dupe label
		errflags.push_back(6);
	  else if ( s == -1 )	// too many symbols
		errflags.push_back(9);
		
	} // end translation
	else
	  outf << "SKIP\n"; // no errors (comment line), so skip it
    }
    else
	outf << "SKIP\n"; // no errors (blank line), so skip it
  } // end START while

  if(!errflags.empty()) {
    pass1Error = false;
	for(int k=0; k!=errflags.size(); ++k)
		outf << " " << errflags[k];
  }

  outf << "\nSYM: " << label << "\nINS: " << instruction << "\nOPD: " <<
	operand << "\nLOC: " << uppercase << hex << locctr << "\n";

  // begin processing the remaining lines
  while(!inf.eof() && !endSet) {

    oldloc = locctr;
    getline(inf, srcLine);
    outf << srcLine << "\n!";

    instruction = "";
    label = "";
    operand = "";
    srcTokens.clear();
    errflags.clear();
    tokenizeStr(srcLine, srcTokens);
    
    goodLine = processLine();

    if(!errflags.empty()) {
        pass1Error = false;
	    for(int k=0; k!=errflags.size(); ++k)
		    outf << " " << errflags[k];
        errflags.clear();
    }

    if(goodLine) 
      outf << "\nSYM: " << label << "\nINS: " << instruction << "\nOPD: " <<
            operand << "\nLOC: " << uppercase << hex << oldloc << "\n";
    else
      outf << "\nSKIP!\n";	// comment, blank or really bad line

  } // end END while

  bool missingAddr = showSymtab();

  progLen = locctr - start;

  if(locctr > 32768 || progLen > 32768)
    errflags.push_back(10);	// program too long

  if(missingAddr)
	errflags.push_back(15);	// symbol missing a label (address)
	
  if(!endSet)
	errflags.push_back(14);	// no END directive

  if(!errflags.empty()) {
    pass1Error = false;
	outf << "\n!";
	for(int k=0; k!=errflags.size(); ++k)
		outf << " " << errflags[k];
    outf << "\nSKIP!";
  }

  inf.close();
  outf.close();

  cout << "  Pass one complete! Intermediate file is " << outfstr<< "\n";

} //end pass1()

void asmType::pass2()
{
  cout << "  Beginning pass two. . .\n";

  string infstr = fname + ".int";
  ifstream inf(infstr.c_str());

  //use the filename (sans any extension) for the listing file
  string outfstr = fname + ".list";
  ofstream outfList(outfstr.c_str());

  //use the filename (sans any extension) for the object file
  outfstr = fname + ".obj";
  ofstream outfObj(outfstr.c_str());
  outfObj.setf(ios::uppercase);

  stringstream textRecord;  // prints to the object file
  stringstream objCode;     // object code represented in hex
  stringstream oField;      // a single field (e.g. instruction, data)

  int fieldLen = 0;             // length of current field read from .int file
  int objLen = 0;               // length of object code in a text record
  bool atEnd = false;		// the END directive has been reached
  bool atRes = false;		// a RESW/RESB directive has been reached
  bool newText = false;		// whether a new text record is needed
				                //	after RESW/RESB
  bool finishLine = false;
  int addr = 0;                 // the value of the 16-bit address field
  int nBytes = 0;
  int indexBit = 32768;         // 2^15 (sets 16th bit)

  while(getline(inf,srcLine)) {

    // original line from assembly source
    // getline(inf, srcLine);
    outfList << srcLine << "\n!";

    // output any errors (following a '!')
    getline(inf, srcLine);
    srcTokens.clear();
    errflags.clear();
    tokenizeStr(srcLine, srcTokens);

    int i = 1;
    int numTokens = srcTokens.size();
    hasError = false;

    if( i < numTokens )
        hasError = true;

    while(i < numTokens) {
        int errNum = strtol(srcTokens[i].c_str(), NULL, 16);
        ++i;

        switch(errNum) {
            case 20: outfList << " |Invalid BYTE operand|";
                break;
            case 19: outfList << " |Invalid RESB operand|";
                break;
            case 18: outfList << " |Invalid RESW operand|";
                break;
            case 17: outfList << " |Invalid WORD operand|";
                break;
            case 16: outfList << " |Invalid END operand|";
                break;
            case 15: outfList << " |Undefined Symbol|";
                break;
            case 14: outfList << " |No END directive|";
                break;
            case 12: outfList << " |Illegal instruction operand|";
                break;
            case 11: outfList << " |Missing instruction or directive|";
                break;
            case 10: outfList << " |Program too long|";
                break;
            case 9: outfList << " |Too many symbols|";
                break;
            case 8: outfList << " |Illegal directive operand|";
                break;
            case 7: outfList << " |Illegal label|";
                break;
            case 6: outfList << " |Duplicate label|";
                break;
            case 5: outfList << " |Invalid decimal operand|";
                break;
            case 4: outfList << " |Invalid hexadecimal operand|";
                break;
            case 3: outfList << " |No START address|";
                break;
            case 2: outfList << " |Illegal operation|";
                break;
            case 1: outfList << " |Missing START directive|";
                break;
            case 0: outfList << " |Missing START label|";
                break;
            default: outfList << " |Unknown error|";
        }

    } // end while (print error messages)

    outfList << "\nOBJ: ";

    // will get either a "SKIP!" or the label ("SYM:")
    getline(inf, srcLine);

    if(srcLine != "SKIP!") {
        
        getline(inf, instruction);
        getline(inf, operand);
        getline(inf, loadAddr);
	    addr = 0;

        if(!hasError) {
            if(operand.length() > 4)
                operand = operand.substr(5, operand.length() - 5);
            else
                operand = "";
            instruction = instruction.substr(5, instruction.length() - 5);
            loadAddr = loadAddr.substr(5, loadAddr.length() - 5);

            if( instruction == "START" ) {
                outfObj.setf(ios::left);
                outfObj << "H" << setw(6) << progName;
                outfObj.setf(ios::right);
                outfObj << hex << setw(6) << setfill('0') << start;
                outfObj << hex << setw(6) << setfill('0') << progLen;
                outfObj << "\n";
		        atRes = false;
            }   // end START check
            else if( instruction == "RESB" || instruction == "RESW" ) {
                fieldLen = 0;
		        if(!atRes) {
			        newText = true;
			        finishLine = true;
		        }
		        atRes = true;
            }   // end RESB/RESW check
            else if( instruction == "BYTE" ) {
		        atRes = false;
		        if(operand[0] == 'C') {	// convert chars to hex
		          fieldLen = (operand.length() - 3) * 2;
		          for(int j = 2; j != operand.length() - 1; ++j) {
			        oField << hex << setw(2) << setfill('0')
				        << int(operand[j]);
		          }
		        }
		        else {	// hex digits are ready
		          fieldLen = operand.length() - 3;
		          oField << uppercase << operand.substr(2, operand.length()-3);
                }

	        }   // end BYTE check
            else if( instruction == "WORD" ) {
		        atRes = false;
                fieldLen = 6;
		        addr = strtol(operand.c_str(), NULL, 10);
		        oField << hex << uppercase << setw(6)
			        << setfill('0') << addr;
            }   // end WORD check
            else if( instruction == "END" ) {
		        atRes = false;
                atEnd = true;
                fieldLen = 0;
            }   // end END check
            else {  // must be a regular instruction
                atRes = false;
		        fieldLen = 6;
                bool isIndexed = false;
                if(instruction != "4C") {
                    if(operand.length() > 1)
                    if(operand.substr(operand.length()-2, 2) == ",X") {
			            operand = operand.substr(0, operand.length()-2);
                        addr = indexBit;
                    }
                    
                    addr += getSymAddr(operand);

                    if(addr == -1)
                        addr = 0;
                }
            
                oField << instruction << hex << uppercase << setw(4)
                    << setfill('0') << addr;

                /* //verbose output
                cout << instruction << " " << hex << addr << "\n";
                cout << oField.str() << "\n";
                */
            }   // end normal instruction check


            if((objLen + fieldLen) > 60 || atEnd || (atRes && finishLine)) {
		        if(atRes)
			        finishLine = false;
		        if(atEnd)
			        objLen += fieldLen;

                textRecord << hex << setw(2) << setfill('0') << uppercase
                    << (objLen / 2) << objCode.str() << endl;
                outfObj << textRecord.str();

                /* //verbose output
                cout << textRecord.str();
                cout << "Len: " << dec << objLen << "\n";
                */
             
                objLen = 0;
            }

            if((objLen == 0 || newText) && !atRes) {
		        // every 2 hex digits is 1 byte
                objCode.str("");
                objCode.clear();
                textRecord.str("");
                textRecord.clear();
                string t = "T";
                textRecord << t << hex << setw(6) << setfill('0')
                    << loadAddr;
		        newText = false;
            }

            objCode << hex << setw(fieldLen) << setfill('0') << oField.str();
            objLen += fieldLen;

            // print object code to listing file
            outfList << hex << setw(fieldLen) << setfill('0') << oField.str();

            oField.str("");
            oField.clear();

        } // end object code creation

        outfList << "   LDADDR: " << loadAddr;

    } // not a skip
    
    outfList << "\n";

  } // end while (reading intermediate file)

  // output symbol table to listing file
  bool missingAddr = false;

  outfList << "\n";
  for(int i=0; i!=1009; ++i)
	if(!SYMTAB[i].label.empty()) {
		if(!missingAddr && SYMTAB[i].address == -1)
			missingAddr = true;
		outfList << SYMTAB[i].label << " @ " << hex <<
			SYMTAB[i].address;
		outfList << " i:" << i << "\n";
	}


  outfList.close();
  inf.close();

  cout << "  Pass two complete! Listing file is " << fname<< ".list\n";

  if(atEnd && !pass1Error) {
    outfObj << "E" << hex << setw(6) << setfill('0') << firstExec;
    outfObj.close();
    cout << "  Object file is " << fname << ".obj\n";
  }
  else {
    outfObj.close();
    string objFile = fname + ".obj";
    remove(objFile.c_str());
    cout << "  Errors prevented object file creation.\n";
  }

} //end pass2()

int asmType::processLine()
{
    int numTokens = srcTokens.size(), s;
    bool hasLabel = true;

    if( numTokens ) { // if line isn't blank, look at it

	if( srcLine[0] != '.' ) {   // line is not a comment, so translate it

	  srcTokens[0] = toUpper(srcTokens[0]);

	  // no label
	  if( srcLine[0] == ' ' || srcLine[0] == '\t' ) { 
    	    hasLabel = false;
	    instruction = srcTokens[0];	
	  }
	  else {
	    if( numTokens > 1 )
	    	instruction = toUpper(srcTokens[1]);
	    else {
		errflags.push_back(11);	// missing instruction
		return 1;
	    }
	
	    srcTokens[0] = toUpper(srcTokens[0]);

	    if(isLabel(srcTokens[0])) {
		label = srcTokens[0];
		s = symtabInsert(label, locctr);
		if( s == 0 )			// dupe label
			errflags.push_back(6);
		else if ( s == -1 )		// too many symbols
			errflags.push_back(9);
	    }
	    else
		errflags.push_back(7);	// illegal label
	  } // end label check

	  // determine instruction & operand
	  instruction = getInstruction(instruction);

	  if( instruction.empty() || instruction == "START") {
		errflags.push_back(2);	// illegal operation
		return 1;
	  }

	  // special case (no operand)
	  if( instruction == "4C" ) {
		locctr += 3;
		return 1;
	  }
	  
 	  if( (numTokens - hasLabel) > 1 )
	  	operand = srcTokens[(1 + hasLabel)];
	  else {
		if( instruction == "END" ) {
			endSet = true;
            errflags.push_back(16); // bad/missing END operand
        }
    	else
		    errflags.push_back(8);	// missing/illegal operand
		return 1;
	  }

	  // handle directives
	  if( instruction == "END" ) {
		endSet = true;
		operand = toUpper(operand);
		if(isLabel(operand)) {
			s = symtabInsert(operand, -1);
			if ( s == -1 )		// too many symbols
				errflags.push_back(9);
            else {
                firstExec = getSymAddr(operand);
            }
		}
		else
			errflags.push_back(7);	// illegal label 
		return 1;
	  }

	 int olen = operand.length();

	  if( instruction == "BYTE" ) {
		if(olen < 4) {
			errflags.push_back(8);	// illegal directive operand
            errflags.push_back(20); // bad BYTE operand
			return 1;
		}

		operand[0] = toupper(operand[0]);
		if(operand[0] == 'C') {
		  if(olen <= 33) {
			if(operand[1] == '\'' && operand[olen-1] == '\'') {
			// add byte for each char between single-quotes
				locctr += (olen - 3);
				return 1;
			}
		  }
		}
		else if(operand[0] == 'X') {
		  operand = toUpper(operand);
		  if(olen <= 35) {
			if(operand[1] == '\'' && operand[olen-1] == '\'') {
			// add byte for every two digits
			  if( isHex( operand.substr(2, olen-3) ) ) {
				locctr = locctr +  ((olen - 3) / 2);
				return 1;
			  }
			  else {
				errflags.push_back(8); // illegal dir operand
                errflags.push_back(20); // bad BYTE operand
				return 1;
			  }
			}
		  }
		}
		
		errflags.push_back(8);	// illegal dir operand
        errflags.push_back(20); // bad BYTE operand
		return 1;
	  } // end BYTE check

	  if( instruction == "WORD" ) {
		int nBytes = strtol(operand.c_str(), NULL, 10);
		if(nBytes != 0 || (nBytes == 0 && operand[0] == '0'))
			locctr += 3;
		else {
			errflags.push_back(8);	// illegal dir operand
            errflags.push_back(17); // bad WORD operand
        }
		return 1;
	  } // end WORD check
	
	  if( instruction == "RESB" ) {
		int nBytes = strtol(operand.c_str(), NULL, 10);
		if(nBytes > 0 && olen < 6)
			locctr += nBytes;
		else {
			errflags.push_back(8);	// illegal dir operand
            errflags.push_back(19); // bad RESB operand
        }
		return 1;
	  } // end RESB check

	  if( instruction == "RESW" ) {
		int nBytes = strtol(operand.c_str(), NULL, 10);
		if(nBytes > 0 && olen < 6)
			locctr += (3 * nBytes);
		else {
			errflags.push_back(8);	// illegal dir operand
            errflags.push_back(18); // bad RESW operand
        }
		return 1;
	  } // end RESW check

	  // handle all other instructions
	  operand = toUpper(operand);
	  string tmpOperand = operand;
		//check operand for indexed, then HEX/SYMBOL
	  if(olen > 2)
		if(operand.substr(olen-2, 2) == ",X")
			operand = operand.substr(0, olen-2);

	  if(isLabel(operand)) {
		s = symtabInsert(operand, -1);
		if ( s == -1 )		// too many symbols
			errflags.push_back(9);
		else
			locctr += 3;
	  }
	  else if(!isAddr(operand))
	  	errflags.push_back(12);	// illegal instruction operand
	  else
		locctr += 3;
	
	  operand = tmpOperand;
	  return 1;	
	} // end translation
	else
	  return 0;
    }
    else
	return 0;

  return 0;
} //end processLine()

int asmType::showSymtab()
{
  bool missingAddr = false;
  ofstream outf("symtab.txt");

  for(int i=0; i!=1009; ++i)
	if(!SYMTAB[i].label.empty()) {
		if(!missingAddr && SYMTAB[i].address == -1)
			missingAddr = true;
		outf << SYMTAB[i].label << " @ " << hex <<
			SYMTAB[i].address;
		outf << " i:" << i << "\n";
	}

  outf.close();
  return (int) missingAddr; 
} //end showSymtab()

int asmType::instructionHash(string key)
{
  int hash = 5381;
  int len = key.length();
  int i = 0;

  if(len > 5)
    return -1;

  for( ; i!=len; ++i)
    hash = ((hash << 5) + hash) + key[i];

  hash += key[0] * 33 % 23 + key[len-1] * 37 % 17 + i * 13 % 7;

  return (hash - 32);
} //end instructionHash()

int asmType::symbolHash(string key)
{
  int hash = 5381;
  int len = key.length();  

  if(len > 6)
    return -1;

  for(int i=0; i!=len; ++i)
    hash = ((hash << 5) + hash) + key[i]; /* hash * 33 + character */

  return hash;
} //end symbolHash()

int asmType::symtabInsert(string key, int value)
{

  int h = symbolHash(key) % 1009;

  if(h < 0)
	h *= -1;

  do {
    if(SYMTAB[h].label.empty()) {
	if(symCount == 500)
    	  return -1;	// cannot add more labels

	SYMTAB[h].label = key;
	SYMTAB[h].address = value;
	++symCount;
	return 1;
    }
    else if(SYMTAB[h].label == key) {
	if(value == -1) {
	  return 1;
	}
	else if(SYMTAB[h].address == -1) {
	  SYMTAB[h].address = value;
	  return 1;
	}
	else {
	  // duplicate label, keep original address
	  return 0;
	}
    }
    h = (h + 7) % 1009;
  } while (true);

  return 0;
} //end symtabInsert()

int asmType::getSymAddr(string key)
{
  int h = symbolHash(key) % 1009;

  if(h < 0)
	h *= -1;

  do {
    if(SYMTAB[h].label.empty())
        return -1;  // symbol not found in SYMTAB
    else if(SYMTAB[h].label == key)
	    return SYMTAB[h].address;

    h = (h + 7) % 1009;
  } while (true);

  return -1;
} //end getSymAddr()

//assume label is upper-case
bool asmType::isLabel(string label) 
{

  int len = label.length();
  if(len == 0 || len > 6)
	return false;
  if(!isalpha(label[0]))
	return false;
  if(!getInstruction(label).empty())
	return false;
  if(len == 1)
	return true;


  for(int i=1; i!=len; ++i)
	if(!isalnum(label[i]))
		return false;
  return true;		
} //end isLabel()

//assume mnemonic is upper-case
string asmType::getInstruction(string mnemonic)
{
  int h = instructionHash(mnemonic) % 500;
  int x = h/22;
  int y = h%22;

  if(x > 21 || x < 0) // out of range
    return "";

  int index = r[x] + y;

  if(index > 30 || index < 0) // out of range
    return "";

  if(OPTAB[index].instruction == mnemonic)
    return OPTAB[index].opcode;

  return "";
} //end getInstruction()

// expects an upper-case string
bool asmType::isHex(string hexstr)
{
  if( hexstr.empty() )
	return false;
  
  int len = hexstr.length();

  if( len > 32 || (len % 2) != 0 )
	return false;

  for(int i=0; i!=len; ++i)
	// digit or A-F
	if( !(isdigit(hexstr[i])) && !(hexstr[i] >= 65 && hexstr[i] <= 70) )
		return false;
  
  return true;
} //end isHex()

bool asmType::isAddr(string addr)
{
  if( addr.empty() )
	return false;

  int len = addr.length();

  if( isalpha(addr[0]) )
	return false;

  for(int i=1; i!=len; ++i)
	// digit or A-F
	if( !(isdigit(addr[i])) && !(addr[i] >= 65 && addr[i] <= 70) )
		return false;
  
  return true;
} //end isAddr()

void tokenize(const char *str, vector<string> &tokens)
{
  bool contWhile;
  bool blankToken;

  do 
  {
    const char *start = str;
    contWhile = true;
    blankToken = true;

    //this inner loop ends when whitespace (delimiter) is found
    //if the first character of a new possible token is a whitespace,
    //  then the token is 'blank' and not added to the vector
    while(*str && contWhile)
    {
	    switch(*str)
	    {
	      case ' ':
	      case '\t':
	      case '\r':
	      case '\v':
	      case '\0':  contWhile = false;
		    break;
	      default:    blankToken = false;
		          ++str;
	    }
    }   

    if(!blankToken)
    	tokens.push_back(string(start, str));

  } while( 0 != *str++);

  //tokendump(tokens); //for testing
} //end tokenize()

void tokenizeStr(string str, vector<string> &tokens)
{
  bool contWhile;
  bool blankToken;
  int i = 0, len = str.length();

  do
  {
	contWhile = true;
	blankToken = true;
	int start = i;

	  while(i != len && contWhile) {
		switch(str[i]) {
			case ' ':
			case '\t':
			case '\r':
			case '\v':
			case '\0':
			case '\n':	contWhile = false;
				break;
			default:	blankToken = false;
					++i;
		}
	  }

	  if(!blankToken)
		tokens.push_back(str.substr(start, i-start));
  } while( len != i++ );

  //tokendump(tokens); // for testing
} //end tokenizeStr()

void tokendump(const vector<string> &V)
{
  cout<<"\n"<<V.size()<<" tokens\n";

  int vit = 0;

  for( ; vit != V.size(); ++vit)
	cout<<"\n"<<V[vit]<<"~"; //adds '~' to check for whitespace tokens
  cout<<"\n";
} //end tokendump()

string toUpper(const string &s)
{
  int len = s.length();
  string upperStr = s;

  for( int i = 0; i < len; i++)
	upperStr[i] = toupper(s[i]);

  return upperStr;
} //end toUpper()

/*
	REVISION HISTORY (dd/mm/yyyy -- v#.# -- summary)

29/01/2013 -- v0.0 -- Created function outlines.

29/01/2013 -- v0.1 -- Completed the following functions:
	welcome(), commandLoop(), commandEval(), tokenize()
	For now commandEval() directs user to placeholder functions.

31/01/2013 -- v0.2 -- Added comments. All simulator commands call a function.

12/02/2013 -- v0.3 -- Replaced switch in commandEval() with string comparisons.
	Added toUpper().

12/02/2013 -- v0.4 -- Assemble will now accept a parameter (filename), check
	if the file exists, and write the file to a new intermediate file.
	Added stubs to help with Pass 1. Current pass1() looks for first
	potential instruction line (not blank/comment line).

20/02/2013 -- v0.5 -- Hash tables for symbols and instructions with relevant
	functions added. Added processLine() stub.

07/03/2013 -- v0.6 -- Finished processLine(). Added showSymtab().

27/03/2013 -- v0.7 -- Began Pass 2 of the assembler. Transferred assembler
	functions and associated variables to a class (asmType). Added more
	error codes and stored complete error messages for printing to the
	listing file.

03/04/2013 -- v0.8 -- Added getSymAddr() to get the address of a symbol and
    completed the pass2() function. It now works for valid assembly source
    files, but has the potential to crash with an erroneous program.

10/04/2013 -- v0.9 -- The program should now handle any assembly program
    with errors in it.

*/