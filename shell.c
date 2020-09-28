#define BUFSIZE 5 
#define DELIM " \t\r\n\a"

#include <stdio.h>
#include <stdlib.h>
#include "utils.c"
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void display_prompt();
char* read_line(void);
char** parse_line(char*);
int run_args(char**);
int execute(char**);
int shell_setenv(char**);
int shell_getenv(char**);
int shell_exit(char**);
int biCount();
	  	
char *builtIns_str[] = { "setenv", "getenv", "exit"};
int (*builtIns_func[])(char**) = {&shell_setenv, &shell_getenv, &shell_exit};

int main ( int argc){//command loop
  	if ( argc == 1){
		int status;
		char *line,*unescapedLine, **args;
		do{
			
			display_prompt();		//display promp to user
			line = read_line();		//read user input
			unescapedLine = unescape(line, stderr);	//unescape user input
			args = parse_line(unescapedLine);	//parse line into args
			status = execute(args);		//execute command/args
			free(line);
			free(unescapedLine);
			free(args);
		} while (status);
	}else{
		fprintf( stderr, "No command line arguements accepted\n");
		exit(1);
  	}
    
}

int execute(char **args){
	int i;
	if (args[0] == NULL){
		return 1;	//empty command, no response
	}

	for(i=0; i<biCount(); i++){	//else compare command to built ins 
		if(strcmp(args[0], builtIns_str[i])==0){
			return (*builtIns_func[i])(args);
		}
	}
				//else run as /bin/(executable)
	return run_args(args);
}

int biCount(){	//func to count built in funcs 
	return sizeof(builtIns_str)/sizeof(char*);
}

int shell_setenv(char **args){
	if(args[2]!=NULL){	//if 3 arguements
		if(args[3]!=NULL){
			for(int i=3; i<sizeof(args) && args[i]!=NULL;i++){//if more than 3 cat trailing args and space between
				strcat(args[2],args[i]);
				strcat(args[2]," ");
			}
		}
		setenv(args[1], args[2], 1);
		return 1;
	} else {
		fprintf( stderr, "setenv only accepts 2 arguements");
	}	
}

int shell_getenv(char **args){
	if(args[1]!=NULL && args[2]==NULL){	//if 2 but not 3 arguements
		char* val = getenv(args[1]);
		printf(val);
		return 1;
	} else {
		fprintf( stderr, "getenv only accepts 1 arguement");
	}
}

int shell_exit(char **args){
	if(args[1]!=NULL && args[2]==NULL){
		int val;
		for(int i=0; i<sizeof(args[1]) && args[1][i]!=NULL; i++){
			val=isdigit(args[1][i]);	//determines if exit arg contains non integers			
			if(val==0){
				return 1;		//return 1 because exit code contained non integer
			}
		}
		val = atoi(args[1]);		//convert to int and exit
		exit(val);
	} else {
		exit(0);
	}
}

int run_args(char **args)
{
	pid_t pid, wpid;	//process id type vars for waiting and current
  	int status;

  	pid = fork();	//fork process and save pid return
  	if (pid == 0) {  	//if child process 
    		if (execvp(args[0], args) == -1) {
      			perror("execution error");
   		}
    		exit(EXIT_FAILURE);
  	} else if (pid < 0) { // Error forking
    		perror("fork error");
  	} else {    // else Parent process, successful fork
    		do {
      			wpid = waitpid(pid, &status, WUNTRACED);	//wait for process state to change
    		} while (!WIFEXITED(status) && !WIFSIGNALED(status));//loop until exited or killed
	}

	return 1;
}


char **parse_line(char *line){
  	int bufsize = BUFSIZE, position = 0;
  	char **tokens = malloc(bufsize * sizeof(char*)), *token;

  	if (!tokens) {	//if allocation returns NULL error
    		fprintf(stderr, "allocation error\n");   		
		exit(EXIT_FAILURE);
  	}

  	token = strtok(line, DELIM);	//load line into token for parsing
	while (token != NULL) {
    		tokens[position] = token;	
		position++;

		if (position >= bufsize) {//reallocate tokens if position is past buffersize	
      			bufsize += BUFSIZE;
      			tokens = realloc(tokens, bufsize * sizeof(char*));
      			if (!tokens) {	//error if tokens is NULL
        			fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
    			}
		}	
    		token = strtok(NULL, DELIM);	//increment token by delimiter
  	}
  	tokens[position] = NULL;	//set last token to NULL
	return tokens;
}


char* read_line(void){
	const int SIZE = 100;
	int bufsize = SIZE, position = 0, c;		
	char *buffer = malloc(sizeof(char) * bufsize);//allocate buffer for input
	
	if (!buffer) {
		fprintf(stderr, "allocation error\n");
		free(buffer);
		buffer = NULL;
		exit(EXIT_FAILURE);
	}

  	while (1) {  // Read a character
   		c = getchar();
		// EOForInput? replace it with a null character and return.
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
      			return buffer;
		} else {
      			buffer[position] = c;
		}
    		position++;

    		// if position is larger than buffersize, reallocate.
    		if (position >= bufsize) {
      			bufsize += SIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "allocation error\n");
				free(buffer);
				buffer = NULL;	
				exit(EXIT_FAILURE);
      			}
    		}
  	}
}		

void display_prompt(){
	static int first_flag = 1;
	if ( first_flag ) {
		system("clear");
		first_flag=0;
	}else{
		printf("\n");
	}

	printf("tonyshell>>");
	return;
}
