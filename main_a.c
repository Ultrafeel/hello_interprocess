#include <stdio.h>
#include <stdlib.h>
//#include "apue.h"
#include <unistd.h>
#include <errno.h>
#include <bits/errno.h>
#include  <stdint.h>
#include  <inttypes.h>
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
    char line[MAXLINE];
    //execlp("sh", "-v", "-c" , //"ls",  
    //        "$PWD/target_bin/bin/"         "main_b",
    //        (char*)0);
    FILE* ppf = popen(//"ls"
            "target_bin/bin/main_b"
            , "w");
    if (NULL == ppf) {
        err_show("popen error");
        exit(1);
    }
    // int fp = fileno(ppf);
    int n = 0;
    int nbytes = 0;
    do {//char buff[BUFF_SIZE] = "";
        //gets( buff);
         printf(__FILE__" op N: %d\n", n);
         puts(" Insert number");
        int32_t i = 0;

        //char *p;

        errno = 0;
        //int scr = scanf("%+"SCNi32, &i);
        char *p = fgets(line, MAXLINE, stdin);
        if (p != 0) {
           printf("read: %s\n", p);
        } else if (errno != 0) {
            err_show("fgets");
            continue;

        } else {
            printf(" fgets err\n");
            continue;
        }


        printf(" fgets get: '%s' str\n", p);
        i = atoi(p);
        printf(" Inserted number: %"PRId32"\n", i);
        //fflush();
        errno = 0;
        nbytes = fprintf(ppf, "%"PRId32"\n", i);
        //nbytes = write( fp,&i, sizeof(i));
        if (nbytes == -1)
            err_show("error pipe fprintf");
        else
            printf(" fprintf transmitted: %+d\n", nbytes);
        fflush(ppf);


    } while ((nbytes != -1));//&&(++n < 5)
         errno = 0;	
	printf(" process a exit\n");
   if (pclose(ppf) == -1)
        err_show("calling pclose");
    exit(0);
}

