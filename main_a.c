#include <stdio.h>
#include <stdlib.h>
//#include "apue.h"
#include <unistd.h>
#include <errno.h>
#include <bits/errno.h>
#include  <stdint.h>
enum {
	BUFF_SIZE = 128,
	MAXLINE = 128
};


typedef __pid_t pid_t;

void err_sys(char const* str)
{
	puts(str);
}
void err_show(char const* str)
{     
    printf("error: %s . erro = %d\n", str, errno);
}


int main(void) {
    //int n = EACCES;

    //int fd[2];
    //pid_t pid; 
    //char line[MAXLINE];
    //execlp("sh", "-v", "-c" , //"ls",  
    //        "$PWD/target_bin/bin/"         "main_b",
    //        (char*)0);
    FILE* ppf = popen(//"ls"
            "target_bin/bin/main_b"
            , "w");
    if (NULL == ppf ) {
        err_show("popen error");
        exit(1);
     }
    int fp = fileno(ppf);
    int nbytes = -1;
    do {//char buff[BUFF_SIZE] = "";
        //gets( buff);
        puts(" Insert number");
        int32_t i = 0;
        scanf("%+"SCNi32, &i);
        //= atoi(buff);
        printf(" Inserted number: %"PRId32"\n", i);
        nbytes = write( fp,&i, sizeof(i));
        if (nbytes == -1)
            err_show("error pipe write");
    } while (nbytes != -1);
    if (pclose(ppf) == -1)
       err_show("calling pclose");
    exit(0);
}

