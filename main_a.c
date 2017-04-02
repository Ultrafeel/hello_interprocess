
#include <stdio.h>
#include <stdlib.h>
//#include "apue.h"
#include <unistd.h>
enum {
	BUFF_SIZE = 128,
	MAXLINE = 128
};


typedef __pid_t pid_t;

void err_sys(char const* str)
{
	puts(str);
}
int main0()
{	
	
	
	
	//char buff[BUFF_SIZE] = "";
	//gets( buff);
	puts(" Insert number" );
	int i = 0;
	scanf("%d", &i);
	//= atoi(buff);
	printf(" Inserted number: %d\n" , i);
	
	
	
    return 0;
}


int main(void)
{
	//int n;
	
	int fd[2];
	pid_t pid; 
	char line[MAXLINE]; 
	if (pipe(fd) < 0)
		err_sys("ошиб­ка вы­зо­ва функ­ции pipe");
	if ((pid = fork()) < 0) 
	{ 
		  err_sys("ошиб­ка вы­зо­ва функ­ции fork");
	} 
	else if (pid > 0) {
		 /* ро­ди­тель­ский про­цесс */ 
		close(fd[0]);
		write(fd[1], "при­вет, МИР\n", 11);
	} else { /* до­чер­ний про­цесс */ 
		close(fd[1]); 
		//MAXLINE,
		sprintf(line,  "%d" ,  fd[0]);
		int nres = execl( "/./main_b", line, (char *)0 );
		if (nres< 0)
			err_sys("ошиб­ка вы­зо­ва функ­ции execlp");
		//n = read(, line, MAXLINE); 
	  
		//write(STDOUT_FILENO, line, n); 
	} 
	exit(0);
}

