/**
 * [Very simple] C to BrainFuck converter
 * Schier Michael
 * Allowed lines in main() are:
 * xc+=y;
 * xc-=y;
 * x[xc]+=y;
 * x[xc]-=y;
 * putchar(x[xc]);
 * x[xc]=getchar();
 * while(x[xc]){
 * }
 */
 
#define bool int
#define false 0
#define true !false
 
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

char* cCode;

void translateCCode()
{
	char* codeP = cCode;
	while(*codeP != EOF && *codeP != '\0')
	{
		if(0 == strncmp(codeP, "xc", 2))
		{
			codeP += 2;
			bool inc = *codeP == '+';
			unsigned short i = 1;
			if(codeP[1] == '=')
			{
				if(sscanf(codeP, "%hu;", &i) != 1)
					err(-1, "Malformed pointer modification code!\n");
			}
				
			while(i-->0)
				if(inc)
					printf(">");
				else
					printf("<");
				
			while(*codeP != ';')
				codeP++;
			codeP++;
		}
		else if(0 == strncmp(codeP, "x[xc]", 5))
		{
			codeP += 5;
			bool inc = *codeP == '+';
			unsigned short i = 1;
			if(codeP[1] == '=')
			{
				if(sscanf(codeP, "%hu;", &i) != 1)
					err(-1, "Malformed value modification code!\n");
			}
				
			while(i-->0)
				if(inc)
					printf("+");
				else
					printf("-");
				
			while(*codeP != ';')
				codeP++;
			codeP++;
		}
		else if(0 == strncmp(codeP, "putchar(x[xc]);", 15))
		{
			codeP += 15;
			printf(".");
		}
		else if(0 == strncmp(codeP, "x[xc]=getchar();", 16))
		{
			codeP += 16;
			printf(",");
		}
		else if(0 == strncmp(codeP, "while(x[xc]){", 13))
		{
			codeP += 13;
			printf("[");
		}
		else if(0 == strncmp(codeP, "while(x[xc]!=0){", 16))
		{
			codeP += 16;
			printf("[");
		}
		else if(0 == strncmp(codeP, "}", 1))
		{
			codeP++;
			if(*codeP != EOF)
				printf("]");
		}
		else
			err(-1, "Could not interpret code correctly: %s\n", codeP);
	}
}

void readCCode(char* filename)
{
	FILE* f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	cCode = (char*) malloc(sizeof(char) * size);
	
	bool start = false;
	int check = 0;
	int i = 0;
	while(true)
	{
		char c = fgetc(f);
		if(c == EOF)
			break;
		if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;
		if(start)			
			cCode[i++] = c;
		else
		{
			if(c == 'm' || c == 'a' || c == 'i' || c == 'n' || c == '(' || c == ')')
				check++;
			else
				check = 0;
			if(check == 6)
			{
				start = true;
				while('{' != fgetc(f)) {} // skip starting '{'
			}
			
		}
	}
	
	fclose(f);
}
 
int main(int argc, char** args)
{
	if(argc != 2)
	{
		printf("C2Brainfuck converter\n");
		printf("Usage: c2bf <infile.c>\n");
		return -1;
	}
	
	readCCode(args[1]);
	translateCCode();


	free(cCode);	
	return 0;
}

