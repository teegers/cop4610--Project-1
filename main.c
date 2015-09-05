#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_setup() {}

void my_prompt()
{
   int homeLen = strlen(getenv("HOME"));
   char* pwd = getenv("PWD");
   char path[100];
   printf("%s", getenv("USER"));
   printf("@");
   printf("%s", getenv("MACHINE"));
   printf(":~");
   strncpy(path, pwd+homeLen, 100);

   printf("%s", path);
   printf("=>");
}

char* my_read()
{
   char in[255];
   fgets(in, sizeof in, stdin);

   if (in[strlen(in)-1] == '\n')
   {
	in[strlen(in)-1] = in[strlen(in)];
   }

   char* res = (char*)malloc(strlen(in)+1);
   strcpy(res, in);
   return res;
}

char** my_parse(char* line)
{

   /* remove leading whitespace */
   while (line[0] == ' ')
   {
	int i;
	for (i = 0; i < strlen(line); i++)
	   line[i] = line[i+1];
   }

   /* remove trailing whitespace */
   while (line[strlen(line)-1] == ' ')
   {
   	line[strlen(line)-1] = line[strlen(line)];
   }

   /* remove extra intermediate whitespace */
   int i;
   int count;

   for (i = 0; i < strlen(line); i++)
   {
	while(line[i]==' ' && line[i+1]==' ')
	{
	   count = i;
	   while(count < strlen(line))
	   {
		line[count] = line[count+1];
		count++;
	   }
	}
   }

   /* add spaces between < > | ? characters */


   /* testing purposes */
   char ** test = {"THIS", "IS", "A", "TEST"};

   return test;
}

void my_free(char* line)
{
   free(line);
}

int main()
{
   my_prompt();

   char * line = my_read();

   char ** cmd = my_parse(line);

   printf("%s\n", line);

   my_free(line);
   return 0;
}
