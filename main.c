#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>

typedef struct {
   char* argv[80];
   int argc;
} UserArgs;
typedef struct dirList {
    char* pathName;
    struct dirList* next;
} Directory;

char* dirString(Directory* root);
char* getWord(char* lineCpy, int num);
char* my_read();
char* parsePath(char* arg);
int addSpaces(char* line);
int contains(UserArgs* line, char in, char out, char pipe, char an);
int getNumWords(char* line);
int my_execute(UserArgs* uargs);
void my_free(char* line, UserArgs* uargs);
void my_prompt();
void popDir(Directory* root);
void printArgs(UserArgs* args);
void pushDir(Directory* root, char* dir);
UserArgs* my_env(char* line);
UserArgs* my_parse(char* line);

/* =============================================== */

int main()
{
   UserArgs* userArgs;
   char* line;
   int exit = 0;

   while(exit==0){
	line = NULL;
	userArgs = NULL;

    line =  my_read();
	userArgs = my_parse(line);
	exit = my_execute(userArgs);
	my_free(line, userArgs);
   }
	
   return 0;
}

/* =============================================== */

void my_prompt()
{
   printf("%s", getenv("USER"));
   printf("@");
   printf("%s", getenv("MACHINE"));
   printf(":");
   printf("%s", getenv("PWD"));
   printf("=>");
}

/* =============================================== */

char* my_read()
{
   char* res = NULL;
   char in[256];

   my_prompt();
   fgets(in, sizeof in, stdin);
   if (in[strlen(in)-1] == '\n')
   {
	in[strlen(in)-1] = in[strlen(in)];
   }
   res = (char*)malloc(strlen(in)+1);
   strcpy(res, in);
   return res;
}

/* =============================================== */

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
	/*pch = strtok(NULL, " ");*/
	wordNumber++;
   }
   return result;
}

/* =============================================== */

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

/* =============================================== */

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

/* =============================================== */

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

/* =============================================== */

UserArgs* my_env(char* line)
{

   char* current = NULL;
   char* letter = NULL;
   char* str = NULL;
   UserArgs* args = NULL;

   int temp = strlen(line);
   int let = 1;
   args =  malloc(sizeof(UserArgs));  /* same as line 146 */ 
   current = (char*)malloc(temp);
   str =  (char*)malloc(temp);
   letter = (char*)malloc(let);
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
	/* calling tokenizer with null argument resumes to the previous
		location */
   }
   args->argv[args->argc] = NULL;

   /* Free up memory */
   current = NULL;
   str = NULL;
   letter = NULL;
   free(current);
   free(str);
   free(letter);

   return args;
}

/* =============================================== */

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

/* =============================================== */

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

/* =============================================== */

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

/* =============================================== */

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

/* =============================================== */

int my_execute(UserArgs* uargs)
{
   double t1,t2;
   int i = 0;
   char* path;
   char c, buffer[256];
   struct timeval time;
   FILE* proc;

   if (strcmp(uargs->argv[0], "exit")==0)
   {
	if (uargs->argc == 1){
	   printf("%s","Closing shell..\n");
	}else{
	   printf("%s","exit: Expression Syntax.\n");
	}
	return 1;
   }else	if (strcmp(uargs->argv[0], "echo") == 0){
	for (i = 1; i<uargs->argc; i++){
		if(i>1){ printf("%s"," "); }
		printf("%s",uargs->argv[i]);
	}
	printf("%s","\n");
	return 0;
   }else if(strcmp(uargs->argv[0], "cd")==0){
	if(uargs->argc > 2){
		printf("cd: Too many arguments.\n");
		return 0;
	}
	if(uargs->argc == 1) {
            uargs->argv[uargs->argc] = "~/";
            uargs->argc+=1;
            uargs->argv[uargs->argc] = NULL;
        }
	path = parsePath(uargs->argv[1]);
	sprintf(buffer, "%s", path);

	if(access(buffer, F_OK) == 0){
        	setenv("PWD", buffer, 1);
	}else{
            fprintf(stderr,
                "%s: XXX or directory.\n",
                uargs->argv[1], buffer);
	}
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

	gettimeofday(&time, NULL);
	t1 = time.tv_sec+(time.tv_usec/1000000.0);

	for(i = 1; i < uargs->argc; i++) {
        	strcpy(uargs->argv[i-1], uargs->argv[i]);
	}
	(uargs->argc)--;
	uargs->argv[uargs->argc] = NULL;
	my_execute(uargs);

	gettimeofday(&time, NULL);
        t2 = time.tv_sec+(time.tv_usec/1000000.0);

	printf("excution time: ");
	printf("%G",t2-t1);
	printf("\n");

        return 0;
   }else if(strcmp(uargs->argv[0], "viewproc")==0){\
	if(uargs->argc == 1) {
            fprintf(stderr, "ERROR: no file argument found.\n");
            return 0;
        }
	sprintf(buffer, "/proc/%s", uargs->argv[1]);
        proc = fopen(buffer, "r");

        if(proc == NULL) {
	    fprintf(stderr, "ERROR: viewproc: cannot access proc/%s: No such file or directory.\n",
	    uargs->argv[1]);
        }else{
            c = fgetc(proc);
            while(c != EOF) {
                fputc(c, stdout);
                c = fgetc(proc);
            }
        }

	return 0;
   }
   
   if (!contains(uargs, '<', '>', '|', '&'))
   {
	   parse_cmd(uargs);
	   
   }else{ 
	   my_redir(uargs);
	   return 0;
   }
   
   if(uargs->argv[0][0] == '/') {
        parse_cmd(uargs);
    }

   return 0;
}

/* =============================================== */

void my_redir(UserArgs* line)
{
   int i; 

   for (i = 0; i < line->argc; i++)
   {
        if (line->argv[i][0] == '<' || line->argv[i][0] == '>')
		{
			my_io(line, i);
			return 0; 
		}
		if (line->argv[i][0] == '|')
		{
			my_pipe(line);
		}
		if (line->argv[i][0] == '&')
		{
			my_zombie(line);
		}
   }
}

/* =============================================== */

void parse_cmd(UserArgs* uargs)
{
	int status;
	pid_t pid = fork();
	
	
	if (pid == -1)
	{
	   printf("Error in creation of child process");
	   return(0);
	}
	else if (pid == 0)
	{
	   /* loop to get a string with ALL $PATH variables */
	   char * pch = NULL;
	   char * cmd = NULL;
	   char cmdC[strlen(uargs->argv[0]) + 1];
	   strcpy(cmdC, uargs->argv[0]);
	 

	   int cmdLen = strlen(uargs->argv[0]);
	   pch = strtok(getenv("PATH"), ":");
	   while (pch != NULL)
	   {
	   	cmd = (char*)malloc(strlen(pch)+cmdLen+2);
   	   	strcpy(cmd, pch);
	   	strcat(cmd, "/");
	   	strcat(cmd, cmdC);
	   	cmd[strlen(pch)+cmdLen+ 1] = '\0';

	   	free(uargs->argv[0]);
	   	uargs->argv[0] = (char*)malloc(strlen(cmd)+1);
	   	strcpy(uargs->argv[0], cmd);

	   	if (execv(uargs->argv[0], uargs->argv) != -1)
	   	{
		   
		   free(uargs->argv[0]);
		   free(cmd);
		   uargs->argv[0] = (char*)malloc(strlen(cmdC)+1);
		   strcpy(uargs->argv[0], cmdC);
		   
		   break; 
	   	}
	
	   	pch = strtok(NULL, ":");
	   	free(cmd);
	   }
	}
	else 
	{
	   waitpid(pid, &status, 0);
	   return 0; 
	}
	
	/* if no command is found */
    fprintf(stderr, "Command not found.\n");
}


/* =============================================== */

void my_io(UserArgs* uargs, int i)
{
 
   /* input redirection */ 
   if (strcmp(uargs->argv[i], "<")==0)
   { 

	   struct stat stat_buf;
		
	   /* loop to get a string with ALL $PATH variables */
	   char * pch = NULL;
	   char * cmd = NULL;
	   char cmdC[strlen(uargs->argv[0]) + 1];
	   char path[1000];
	   
	   strcpy(path, getenv("PATH"));

	   strcpy(cmdC, uargs->argv[0]);

	   int cmdLen = strlen(uargs->argv[0]);
	   pch = strtok(path, ":");
	   while (pch != NULL)
	   {
	   	cmd = (char*)malloc(strlen(pch)+cmdLen+2);
   	   	strcpy(cmd, pch);
	   	strcat(cmd, "/");
	   	strcat(cmd, cmdC);
	   	cmd[strlen(pch)+cmdLen+ 1] = '\0';

	   	free(uargs->argv[0]);
	   	uargs->argv[0] = (char*)malloc(strlen(cmd)+1);
	   	strcpy(uargs->argv[0], cmd);
		
		stat(uargs->argv[0], &stat_buf); 
		int fd = S_ISREG(stat_buf.st_mode);

	   	if(fd == 1)
	   	{
			pid_t pid2 = fork(); 
			
			  if(pid2 == 0)
			  {
				  char *more_args[] = {uargs->argv[0], NULL}; 
				  int in = open(uargs->argv[i + 1], O_RDONLY);
				  
				  close(STDIN_FILENO);
				  
				  dup(in);	
				  close(in);
				  
				  execv(uargs->argv[0], more_args);
				 
				  free(uargs->argv[0]);
				  free(cmd);
				  uargs->argv[0] = (char*)malloc(strlen(cmdC)+1);
				  strcpy(uargs->argv[0], cmdC);
				  exit(0);
				 
				 
			  }else if(pid2 > 0){
				  waitpid(pid2, NULL, 0);
			  }else{
				  printf("error message");
			  }
			  
		   break;
	   	}
	
	   	pch = strtok(NULL, ":");
	   	free(cmd);
	   }   
	}
}

/* =============================================== */

void my_pipe(UserArgs* line)
{
	
}

/* =============================================== */

void my_zombie(UserArgs* line)
{
	
}

/* =============================================== */

int contains(UserArgs* line, char in, char out, char pipe, char an)             
{
   int i;
   for (i = 0; i < line->argc; i++)
   {
        if (line->argv[i][0] == '<' || line->argv[i][0] == '>'                  
                || line->argv[i][0] == '|' || line->argv[i][0] == '&')          
           return 1;
   }
   return 0;
}


/* =============================================== */

void my_free(char* line, UserArgs* uargs)
{
   int i;
   for(i=0; i<uargs->argc; i++){
	uargs->argv[i] = '\0';
   }
   free(uargs);
   free(line);
}

/* =============================================== */

void printArgs(UserArgs* args){
   int i;
   for(i = 0; i<80; i++) {
	if(args->argv[i] != '\0'){
            printf("%s",args->argv[i]);
	    printf("%s"," ");
	}else{
	    return;
	}
    }
}
/* =============================================== */