#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <bits/errno.h>
#include  <stdint.h>
#include  <inttypes.h>
#include <signal.h>
#include <string.h>
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
    printf("error: %s . errno = %d\n", str, errno);
}

volatile sig_atomic_t terminate_flag = 0;
void terminator_sig_hndlr(int sn)
{
	printf("\n terminator_sig_hndlr, PID = %d, signum = %d\n", getpid(), sn);
	terminate_flag = 1;
}

char pidEnv[100];
int main(void) {

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sigemptyset(&sa.sa_mask);
	sa.sa_handler = &terminator_sig_hndlr;
	if (sigaction(SIGTERM, &sa, NULL) < 0) {
		err_show("ошибка вызова функции sigaction(SIGTERM)");
		return 1;
	}
	sa.sa_handler = &terminator_sig_hndlr;
	if (sigaction(SIGINT, &sa, NULL) < 0) {
		err_show("ошибка вызова функции sigaction(SIGTERM)");
		return 1;
	}

    char line[MAXLINE];
 
	printf(  " a:Proc A PID=%d\n", getpid());
	
	//это будет считано в proc B.
	snprintf( pidEnv, 100, "PPID=%d", getpid());
	putenv(pidEnv);
	
    FILE* ppf = popen("target_bin/bin/main_b" , "w");
    if (NULL == ppf) {
		err_show("popen error");
		exit(1);
	}

	int nbytes = 0;
	while ((nbytes != -1) && !terminate_flag) {

		puts(" Insert number");
		int32_t i = 0;

		errno = 0;

		char *p = fgets(line, MAXLINE, stdin);
		if (p != 0) {
			printf("read: %s\n", p);
		} else if (errno != 0) {
			if (EINTR == errno)
				continue;
			err_show("fgets");
			break;
		}


		printf(" fgets get: '%s' str\n", p);
		i = atoi(p);
		printf(" Inserted number: %"PRId32"\n", i);

		errno = 0;
		nbytes = fprintf(ppf, "%"PRId32"\n", i);
		//nbytes = write( fp,&i, sizeof(i));
		if (nbytes == -1)
			err_show("error pipe fprintf");
		else
			printf(" fprintf transmitted: %+d\n", nbytes);
		fflush(ppf);


	};

	printf(" process a exit\n");

	if (pclose(ppf) == -1) {
		err_show("calling pclose");
		return -1;
	}
	exit(0);
}

