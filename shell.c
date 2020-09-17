#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void display_prompt();
void read_command( char*, char* );

int main ()
{//command loop
	char* command = NULL;
	char* longer_command = NULL;
	char* parameters[20];

	while(1){				//repeating loop
		display_prompt();		//display promp to user
		read_command ( command, parameters ); //read input from user terminal
	
	//	if ( fork() != 0 ){
	//		wait ( NULL );
	//	}else{
	//		execv(command, parameters); //execute command
	//	}
	//
	//	}	
	//	if ( strcmp ( command, "exit" ) == 0 ){
	//		break;
	//	}
	}
}


void read_command( char* c, char* p ){

}


void display_prompt(){
	static int first_flag = 1;
	if ( first_flag ) {
		system("clear");
		first_flag=0;
	}

	printf("tonyshell>>");
	return;
}
