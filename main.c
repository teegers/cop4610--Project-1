#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_setup() {}

void my_prompt()
{
   printf("%s", getenv("USER"));
   printf("@");
   printf("%s", getenv("MACHINE"));
   printf(":");
   printf("%s", getenv("PWD"));

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

char * getWord(char * lineCpy, int num)
{
   char line[strlen(lineCpy)+1];
   strcpy(line, lineCpy);

   int wordNumber = 0;
   char * pch;
   char * result;
   pch = strtok(line, " ");

   while(pch != NULL)
   {
	if (wordNumber == num)
	{
	   result = (char*)malloc(strlen(pch)+1);
	   strcpy(result, pch);

	   return result;
	}

	pch = strtok(NULL, " ");

	wordNumber++;
   }

   return result;
}

int getNumWords(char* line)
{
   int count, i;
   char lastChar;
   count = 0;
   if (strlen(line) <= 0)
	return count;
   else
	count = 1;

   for (i = 0; i < strlen(line); i++)
   {
	if (line[i] == ' ')
	   count++;
   }

   return count;
}

int addSpaces(char* line)
{
   int i;
   int k;

   char cpy[strlen(line+1)];
   strcpy(cpy, line);

   for (i = 0; i < strlen(line); i++)
   {
	if (line[i] == '<' || line[i] == '>'
	   || line[i] == '|' || line[i] == '&')
	{
	   if (line[i+1] != ' ' && line[i+1] != '\0')
	   {

		free(line);
		line  = (char*)malloc(strlen(cpy)+2);
		strcpy(line, cpy);

		for(k=strlen(cpy); k > i; k--)
		{
		   line[k+1] = line[k];
		}
		line[i+1] = ' ';
		return 1;
	   }
	   if (i != 0 && line[i-1] != ' ')
	   {
		free(line);
		line = (char*)malloc(strlen(cpy)+2);
		strcpy(line, cpy);


		for(k=strlen(cpy); k >= i; k--)
		{
		   line[k+1] = line[k];
		}

		line[i] = ' ';
		return 1;
	   }
	}
   }
   return 0;
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

   /* add spaces where they need to go */
   while(addSpaces(line));

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

   /* set each word to separate string array */
   int numWords = getNumWords(line);

   char ** cmd = (char**)malloc(sizeof(char*) * (numWords + 1));

   for (i = 0; i < numWords; i++)
	cmd[i] = getWord(line, i);
   
   /*printf("%s\n", getWord(line, 1));*/

   cmd[numWords] = "NULL";

   return cmd;
}

void my_execute(char** cmd)
{

   if (strcmp(cmd[0], "exit")==0)
   {
	if (strcmp(cmd[1], "NULL")==0)
	{
	   printf("Closing shell..\n");
	   exit(0);
	}
	else
	{
	   printf("exit: Expression Syntax.\n");
	}
   }
   if (strcmp(cmd[0], "echo") == 0)
   {
	printf("You got it");
   }

}

void free2D(char ** cmd)
{
   int i;
   for (i = 0; cmd[i] != "NULL"; i++)
	free(cmd[i]);

   free(cmd);
}

void my_free(char* line, char** cmd)
{
   free2D(cmd);
   free(line);
}

void print2D(char** cmd)
{
   int i;
   for (i = 0; cmd[i] != "NULL"; i++)
   {
	printf("%s", cmd[i]);
	if (cmd[i+1] != "NULL")
	   printf("_");
   }
   printf("\n");
}

int main()
{
   my_prompt();

   char * line = my_read();

   /*printf("%s\n", line);*/

   char ** cmd = my_parse(line);

   /*print2D(cmd);*/

   my_execute(cmd);

   my_free(line, cmd);
   return 0;
}
