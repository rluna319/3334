#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int main()
{
	char operand[6] = "X'F1'";	
	int oplen = strcspn(operand, "\0");	//operand length
	char tmp[oplen];	//tmp str to hold the chars within the single quotes	
	int locctr = 0;

	printf("oplen = %d\n\n", oplen);

	for (int i = 2, j = 0; i < oplen - 1 && j < oplen; i++){
			tmp[j] = operand[i];
			j++;
		}
	printf("tmp = %s\n\n", tmp);
	//int numlen = strcspn(tmp, "\'");
	int num = strtol(tmp, NULL, 16);
	printf("num = %d\n\n", num);
	if (operand[0] == 'C') locctr += num;
	if (operand[0] == 'X') locctr += num/2;	

	printf("operand = %s\n\nlocctr = %d\n\n", operand, locctr);
	
	return 0;
}