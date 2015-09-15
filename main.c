#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct {
   char* argv[80];
   int argc;
} UserArgs;

typedef struct dirList {
    char* pathName;
    struct dirList* next;
} Directory;

UserArgs* my_env(char* line);

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
   char* res = NULL;
   char in[256];
   fgets(in, sizeof in, stdin);

   if (in[strlen(in)-1] == '\n')
   {
	in[strlen(in)-1] = in[strlen(in)];
   }

   res = (char*)malloc(strlen(in)+1);
   strcpy(res, in);


   return res;
}

char* getWord(char * lineCpy, int num)
{
   int wordNumber = 0;
   char line[strlen(lineCpy)+1];
   char * pch = NULL;
   char * result = NULL;
   
   strcpy(line, lineCpy);
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

UserArgs* my_parse(char* line)
{
   int i, count;
   UserArgs* args = NULL;

   args =  malloc(sizeof(UserArgs));
   args->argc = 0;

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
   
   args = my_env(line);

   return args;
}

char* dirString(Directory* root) {
    Directory* node = NULL;
    char buffer[256];
    char* result = NULL;
        
    buffer[0] = '\0';
    node = root;
        
    while(node != 0) {
        sprintf(buffer, "%s/%s", buffer, (*node).pathName);
        node = node->next;
    }
   
    result = (char*)malloc(strlen(buffer)+1);
    strcpy(result, buffer);
    
    return result;
}

void pushDir(Directory* root, char* dir) {
    Directory* node = NULL;
   
    if(root->pathName == 0) {
        root->pathName = dir;
        return;
    }
    node = root;
   
    /* Moves to most recent directory */
    while( node->next != 0 ) {
        node = node->next;
    }
    node->next = malloc(sizeof(Directory));
    node = node->next;
    node->pathName = dir;
}
    
void popDir(Directory* root) {
    Directory* node = NULL;
           
    node = root;
    if(root->next == NULL) {
        node->pathName = NULL;
        free(node->pathName);
        return;
    }
   
    /* verify we dont delete the root */
    while( node->next->next != 0 ) {
        node = node->next;
    }
    node->next = NULL;
    free(node->next);
}

char* parsePath(char* arg) {
    Directory* root = NULL;
    char path[4096]; 
    char* current = NULL;
    root = malloc(sizeof(Directory));
    
    if(arg[0] == '/') {
        return arg;
    }else if(arg[0] == '.' && arg[1] == '/') {
        /* ./ :expands to working directory */
        sprintf(path, "%s/%s", getenv("PWD"), arg+1);
    }else if(arg[0] == '~') {
        /* ~ :expands to the $HOME directory */
        sprintf(path, "%s/%s", getenv("HOME"), arg+1);
    }else{
        /* expands to the Relative path */
        sprintf(path, "%s/%s", getenv("PWD"), arg);
    }
        
    current = strtok(path, "/");
    while(current != NULL) { 
        if(strcmp(current, "..") == 0) {
            popDir(root);
        }else if(strcmp(current, ".") != 0) {
            pushDir(root, current);
        }
    
        current = strtok(NULL, "/");
    }
    
    return dirString(root);
}

int my_execute(UserArgs* uargs)
{
   int i = 0;
   char* path;

   if (strcmp(uargs->argv[0], "exit")==0)
   {
	if (uargs->argc == 1){
	   printf("Closing shell..\n");
	}else{
	   printf("exit: Expression Syntax.\n");
	}
	return 1;
   }else	if (strcmp(uargs->argv[0], "echo") == 0){
	for (i = 1; i<uargs->argc+1; i++){
		if(i>1){ printf(" "); }
		printf("%s", uargs->argv[i]);
	}
	printf("\n");
	return 0;
   }else if(strcmp(uargs->argv[0], "cd")==0){
	if(uargs->argc == 1) {
            uargs->argv[uargs->argc] = "~/";
            uargs->argc+=1;
            uargs->argv[uargs->argc] = NULL;
        }
	path = parsePath(uargs->argv[1]);
	/*if we need to check if directory exists, check below value for access*/
        setenv("PWD", path, 0);
        
        return 0;	
   }else if(strcmp(uargs->argv[0], "clear") == 0){
	/* Source: 
	stackoverflow.com/questions/1348563/clearing-output-of-a-terminal-program-linux-c-c
	*/
   	fprintf(stdout, "\033[2J\033[1;1H");
        rewind(stdout);
        return 0;
   }else if(strcmp(uargs->argv[0], "time")==0){
        if(uargs->argc == 1) {
            fprintf(stderr, "ERROR: Syntax Error.\n");
            return 0;
        }
	
	/*
	TODO:
	t1 = get current time
	shift all arguments left
	complete rest of arguments
	t2 = get current time
	execution time = t2-t1	
	*/

        return 0;
   }else if(strcmp(uargs->argv[0], "viewproc")==0){
	if(uargs->argc == 1) {
            fprintf(stderr, "ERROR: no file argument found.\n");
            return 0;
        }
   /*
	TODO:
	PERFORM ALL NECESSARY ACTIONS
   */
   }

/*
   if(uargs->argv[0][0] == '/') {
        int pid;
        pid = fork();
        if(pid == 0) {
            if(execv(uargs->argv[0], uargs->argv)) {
                fprintf(stdout, "execv failed with error %d %s\n", errno, strerror(errno));
            }
            exit(0);
        }else { wait(&pid); }
        return 0;
    }
*/

   /* if no command is found */
   fprintf(stderr, "Command not found.\n");
   return 0;
}

UserArgs* my_env(char* line)
{
	char* current = NULL;
	char* letter = NULL;
	char* str = NULL; 
	UserArgs* args = NULL;

	args =  malloc(sizeof(UserArgs));
	current =  malloc(sizeof(UserArgs));
	str =  malloc(sizeof(UserArgs));
	args->argc = 0;
	
	/* set each word to separate string array */
	
   current = strtok(line," ");  /* Puts one word from line into current */
   while(current != NULL) {  
	letter = current[0];
	    if(letter == '$'){
			str = current + 1; 
			
			if(getenv(str) == NULL){
				printf("%s", str); 
				printf(": Undefined variable ");
			
				current = strtok(NULL, " ");
			}
			else{
				args->argv[args->argc] = getenv(str);
				args->argc += 1;
				current = strtok(NULL, " ");
			}
		}
		else{
			args->argv[args->argc] = strdup(current);
			args->argc += 1;
			current = strtok(NULL, " ");
		}
	/* calling tokenizer with null argument resumes to the previous location */
    }
   args->argv[args->argc+1] = NULL;
   
   return args;
}

void my_free(char* line, UserArgs* uargs)
{
   free(uargs);
   free(line);
}

void printArgs(UserArgs* args){ 
   int i = 0;
   for(i = 0; i<args->argc; i++) {  
       printf("%s", args->argv[i]);
	printf(" ");
    }
} 

int main()
{
   UserArgs* userArgs = NULL;
   char * line = NULL;
   int exit = 0;
   
   while(exit==0){
	my_prompt();
	line = my_read();   
	userArgs = my_parse(line);
	exit = my_execute(userArgs);
	my_free(line, userArgs);
   }
        
   return 0;
}