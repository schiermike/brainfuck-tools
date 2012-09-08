/**
 * BFI - The Brainfuck Interpreter
 * Schier Michael
 */
 
#include <stdio.h>
#include <string.h>

#define bool int
#define false 0
#define true !false
#define null NULL

#define MAXM 65536

unsigned short codeIndex = 0;
unsigned short memoryIndex = 0;
unsigned char code[MAXM];
unsigned int memory[MAXM];
bool debug;

void printCode()
{
	int i = 0;
	while(true)
	{
		char c = code[i++];
		if(c == '#')
			continue;
		if(c == 0)
			break;
		putchar(c);
	}
}

void printMemoryDump()
{
	int i;
	fprintf(stderr, "DUMP:");
	for(i = 0; i < 200; i++)
	{
		if(i%10 == 0)
			fprintf(stderr, "\n");
		fprintf(stderr, "%03d>%03u  ", i, memory[i]);
	}
	fprintf(stderr, "\n");
}

void skipLoop()
{
	int parenCount = 0;
	while(true)
	{
		switch(code[codeIndex])
		{
			case '[':
				parenCount++;
				break;
			case ']':
				parenCount--;
				if(parenCount == 0)
					return;
				break;
		}
		codeIndex++;
	}
}

void reverseLoop()
{
	int parenCount = 0;
	while(true)
	{
		switch(code[codeIndex])
		{
			case ']':
				parenCount++;
				break;
			case '[':
				parenCount--;
				if(parenCount == 0)
					return;
				break;
		}
		codeIndex--;
	}
}

void run()
{
	codeIndex = -1;
	memoryIndex = 0;
	
	while(true)
	{
		codeIndex++;
		if( code[codeIndex] == '#')
		{
			printMemoryDump();
			continue;
		}
	
		if(debug) fprintf(stderr, "%hu:\t%c\t", codeIndex, code[codeIndex]);
		if(debug) fprintf(stderr, "m[%hu]=%u", memoryIndex, memory[memoryIndex]);
		switch(code[codeIndex])
		{
			case '>':
				memoryIndex++;
				break;
			case '<':
				memoryIndex--;
				break;
			case '+':
				memory[memoryIndex]++;
				break;
			case '-':
				memory[memoryIndex]--;
				break;
			case '.':
				putchar(memory[memoryIndex]);
				break;
			case ',':
				memory[memoryIndex] = getchar();
				if(memory[memoryIndex] == EOF)
				{
					fprintf(stderr, "Could not read more from standard input!\n");
					return;
				}
				break;
			case '[':
				if(!memory[memoryIndex])
					skipLoop();
				break;
			case ']':
				if(memory[memoryIndex])
					reverseLoop();
				break;
			default:
				return;
		}
		if(debug) fprintf(stderr, "\t->\tm[%hu]=%u\n", memoryIndex, memory[memoryIndex]);
	}
}

bool readFile(const char* fileName)
{
	FILE* f = fopen(fileName, "r");
	if(!f)
	{
		fprintf(stderr, "Could not open file '%s' for reading!\n", fileName);
		return false;
	}
	
	memset(code, 0, MAXM);
	memset(memory, 0, MAXM);
	
	bool skip = false;
	int line = 1;
	int codeI = 0;
	int parenCount = 0;
	while(true)
	{
		char c = fgetc(f);
		if(c == EOF)
			break;
		
		// skip comments
		if(skip)
		{
			if(c == '\n')
			{
				skip = false;
				line++;
			}
			continue;
		}
		
		switch(c)
		{
			case '>':
			case '<':
			case '+':
			case '-':
			case '.':
			case ',':
			case '#':
				code[codeI++] = c;
				break;
			case '[':
				code[codeI++] = c;
				parenCount++;
				break;
			case ']':
				code[codeI++] = c;
				parenCount--;
				if(parenCount < 0)
				{
					fprintf(stderr, "Too many closing parenthesis at line %d!\n", line);
					return false;
				}
				break;
			case '/':
				skip = true;
				break;
			case ' ':
			case '\t':
			case '\r':
				// ignore these characters
				break;
			case '\n':
				line++;
				break;
			default:
				fprintf(stderr, "Illegal character '%c' at line %d!\n", c, line);
				return false;
		}
	}
	if(parenCount > 0)
	{
		fprintf(stderr, "Too many opening parenthesis in the code!\n");
		return false;
	}
	
	return true;
}
 
int main(int argc, char** args)
{
	if(argc == 1)
	{
		printf("***********************************\n");
		printf("* BFI - The Brainfuck Interpreter * \n");
		printf("***********************************\n");
		printf("Usage: bfi [-d] <bf-code-file> [-s]\n");
		printf("\n");
		return -1;
	}
	
	int i = 1;
	debug = false;
	if( 0 == strncmp(args[i], "-d", 2) )
	{
		debug = true;
		i++;
	}
	
	if(!readFile(args[i]))
		return -1;
		
	i++;
	if(i < argc && !strncmp(args[i], "-s", 2))
	{
		printCode();
	}
	else
		run();
	
	return 0;
}

