#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

char myToken[80];
void myTokenize(char *line);
void SmartTokenize(char *line);

char tokens[3][50];


int main()
{
  FILE *source;
  source = fopen("source.asm", "r");
  char line[80];
  while(fgets(line, 80, source))
  {
	//     printf("%s", line);
	//	myTokenize(line);
	SmartTokenize(line);
  } 


  return 0;
}


void myTokenize(char *line)
{
	int tokenCount = 0;
	int lineIndex = 0;

	if (line[0] == '.') 
		return;

	//tokenize loop
	for (int tok = 0; tok < 3;)
	{
		bool continueLoop = true;
		bool isEnd = false;
		int startingIndex = lineIndex;

		while(continueLoop)
		{
			switch(line[lineIndex])
			{
				case ' ':	//if space is hit
				case '\t':	//if tab is hit
				case '\r':
				case '\v':
				case '\0':	//if empty char is hit
					continueLoop = false;
					break;
				case '\n':	//if newline is hit
					isEnd = true;
					continueLoop = false;
					break;
				default:
					++lineIndex;
			}
		}

		if(lineIndex - startingIndex > 0) {
			tok++;
			memset(&myToken[0], 0, sizeof(myToken));  // clear token character array
			memcpy(myToken, &line[startingIndex], lineIndex - startingIndex);
			printf("%d: %s\t\t", tok, myToken);			
		}
					
		++lineIndex;

		if(isEnd || tok == 3) {
			printf("\n");
			break;	
		}
	}
}

void SmartTokenize(char *line)
{
	int tokenCount = 0;
	int lineIndex = 0;
	int tokenIndex = 0;
	memset(&tokens, 0, 150);  // clear tokens

	if (line[0] == '.') 
		return;
	
	if (line[0] == ' ' || line[0] == '\t')
		tokenIndex = 1;

	//tokenize loop
	for (; tokenIndex < 3;)
	{
		bool continueLoop = true;
		bool isEnd = false;
		int startingIndex = lineIndex;

		while(continueLoop)
		{
			switch(line[lineIndex])
			{
				case ' ':	//if space is hit
				case '\t':	//if tab is hit
				case '\r':
				case '\v':
				case '\0':	//if empty char is hit
					continueLoop = false;
					break;
				case '\n':	//if newline is hit
					isEnd = true;
					continueLoop = false;
					break;
				default:
					++lineIndex;
			}
		}

		if(lineIndex - startingIndex > 0) {
			tokenIndex++;			
			memcpy(&tokens[tokenIndex][0], &line[startingIndex], lineIndex - startingIndex);
			printf("%d: %s\t\t", tokenIndex, tokens[tokenIndex]);			
		}
					
		++lineIndex;

		if(isEnd || tokenIndex == 3) {
			printf("\n");
			break;	
		}
	}




}
