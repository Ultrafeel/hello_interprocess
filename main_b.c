/*
 * main_b.c
 * 
 * Copyright 2017 MML
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <wchar.h>


#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

enum {
	BUFF_SIZE = 128,
	MAXLINE = 128
};


void err_sys(char const* str)
{
	puts(str);
}

void err_show(char const* str)
{
	printf("error: %s . errno = %d, err description: %s\n", str, errno, strerror(errno));
}
void delSHM();
void err_showE(char const* str)
{
	err_show(str);
	printf("process will exit due to error.\n");
	//ничего не делает, если память ещё не выделена.
	delSHM();
	exit(1);
}


typedef unsigned char bool;
enum {
	false = 0,
	true = 1
};

typedef struct _TShMem {
	volatile sig_atomic_t flag;
	sem_t buff_is_free_sem;
	sem_t buff_is_full_sem;

	long unsigned val;
} TShMem;

enum {
	MALLOC_SIZE2 = sizeof(TShMem)
};
volatile int shmid = 0;
TShMem  * ptrShMem = 0;

	
void delSHM()
{
	if (0 != shmid) {
		
		sem_destroy(&ptrShMem->buff_is_free_sem);
		sem_destroy(&ptrShMem->buff_is_full_sem);

		if (shmctl(shmid, IPC_RMID, 0) < 0)
			err_sys("ошибка вызова функции shmctl");
		else
			shmid = 0;
	}
}

int initSharedMem(void)
{

	errno = 0;
	if ((shmid = shmget(IPC_PRIVATE, MALLOC_SIZE2, IPC_CREAT | IPC_EXCL | 0666)) < 0)
		err_showE("ошибка вызова функции shmget");
	errno = 0;
	if ((ptrShMem = shmat(shmid, 0, 0)) == (void *) - 1)
		err_showE("ошибка вызова функции shmat");
	printf("сегмент разделяемой памяти присоединен в адресах от %p до %p\n",
		(void *) ptrShMem, (void *) ptrShMem + MALLOC_SIZE2);

	errno = 0;
	// Mac OS X does not actually implement sem_init()
	if (sem_init(&ptrShMem->buff_is_free_sem, 1, 1) == -1)
    { printf("sem_init: failed: %s\n", strerror(errno)); }
	
	if (sem_init(&ptrShMem->buff_is_full_sem, 1, 0) == -1)
    { printf("sem_init: failed: %s\n", strerror(errno)); }
	
	ptrShMem->flag = false;

	return(0);
}


int timedWaitSem(sem_t * sem)
{
	int err = -1;
	struct timespec tout;
	struct tm *tmp;
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	char buf[64];
	strftime(buf, sizeof(buf), "%r", tmp);
	wprintf(L"те­ку­щее вре­мя: %s\n", buf);
	tout.tv_sec += 5; /* 10 се­кунд, на­чи­ная от те­ку­ще­го вре­ме­ни */

	err = sem_timedwait(sem, &tout);
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	wprintf(L"те­ку­щее вре­мя: %s\n", buf);
	if (err == 0)
		wprintf(L"sem заперт!\n");
	else
	{
		wprintf(L"не по­лу­чи­лось по­втор­но за­пе­реть sem: %d\n", err);
		if (ETIMEDOUT == err)
			wprintf(L"ETIMEDOUT");
		printf("\n");
			
	}
	return err;
}


int writeToShared(int isqr)
{
	int retc = timedWaitSem(&ptrShMem->buff_is_free_sem);
	if (0 != retc)
		return retc;

	if (ptrShMem->flag)
		printf("sh mem is busy! override !\n");
	
	

	ptrShMem->flag = true;
	ptrShMem->val = isqr;
	
	sem_post(&ptrShMem->buff_is_full_sem);
	
	return 0;
}

volatile sig_atomic_t  c_recive_value = false;
volatile int gSqureCRecieved = -1;
pthread_t c2_tid;

bool CheckAndPrint()
{

	if (c_recive_value) {
		printf(" c:value = %d\n", gSqureCRecieved);
		c_recive_value = false;
		return true;
	} else {
		//printf(" c:strange call to ");
		//printf(__func__);
		//printf(" function\n");
		return false;
	}
}

enum {
	signumForC2Notification = SIGUSR2
};

enum {
	MAGIC_NUMBER = 100
};
void sig_recieve_mem_handle(int a)
{	
	//printf(" c: sig_recieve_mem_handle thread id = %d - is created tread = %s\n",
	//	(int)pthread_self(),
	//	( pthread_equal(pthread_self(), c2_tid)?"true":"false"));	
	c_recive_value = true;
	//CheckAndPrint();
}

volatile sig_atomic_t terminate_flag = 0;
void terminator_sig_hndlr(int sn)
{
	printf("\n terminator_sig_hndlr, PID = %d, signum = %d\n", getpid(), sn);
	terminate_flag = 1;
}

void * c2_thr_fn(void *arg)
{
    printf(" c:  thread 2 ");

	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	//struct timespec tsret;
	while (!terminate_flag) {
		//с данным кодом "I am alive будет" выполнятся при прерывании, 
		// а не только периодически.
		if (!CheckAndPrint())
			printf(" c:I am alive\n");

		//nanosleep прерывается по signal.
		if (-1 == nanosleep(&ts, NULL)) {
			if (EINTR != errno) {
				err_show(" nanosleep");
				break;
			}
		}
	};
	printf(" c: C2 exits\n");
    return((void *)0);
}

int createTc2(void)
{
	int err = 0;
	err = pthread_create(&c2_tid, NULL, c2_thr_fn, NULL);
	if (err != 0) {
		printf(" c:can't create thread %d", err);
		exit(1);
	}
	return err;
}



void procC(const pid_t bpid) {
	puts("proc C started!\n");
	
	createTc2();
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = &sig_recieve_mem_handle;
	if (sigaction(signumForC2Notification, &sa, NULL) < 0)
		err_showE("ошибка вызова функции sigaction()");
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = &terminator_sig_hndlr;
	if (sigaction(SIGTERM, &sa, NULL) < 0)
		err_showE("ошибка вызова функции sigaction(SIGTERM)");
	sa.sa_handler = &terminator_sig_hndlr;
	if (sigaction(SIGINT, &sa, NULL) < 0)
		err_showE("ошибка вызова функции sigaction(SIGTERM)");

	while (!terminate_flag) {
		errno = 0;
		int ret = sem_wait(&ptrShMem->buff_is_full_sem);
		if (ret == -1) {
			if (EINTR == errno)
				continue;
			err_show(" c:sem_wait");
			break;
		}
		
		if (ptrShMem->flag) {

			//c_recive_value = true;
			gSqureCRecieved	= ptrShMem->val;

			ptrShMem->flag = false;
	
			sem_post(&ptrShMem->buff_is_free_sem);
			
			pthread_kill( c2_tid, signumForC2Notification);
			if (MAGIC_NUMBER == gSqureCRecieved)
				kill(bpid, SIGUSR1);	//getppid()
			fflush(stdout);
		} else
			printf(" c: flag false??");
		
	} ;

	printf(" exit c start!!\n");
	//wait second thread;
	pthread_join(c2_tid, NULL);
	printf(" exit c!!\n");
	exit(0);
}

int main(int argc, char **argv)
{
	puts(__FILE__" Hello");

	if (argc > 0) {
		printf(" Hello, arg = %s\n", argv[0]);

	}
	char const * procA_pid = getenv("PPID");
	pid_t pid_a;
	if (procA_pid)
	{
		printf("proc a pid = %s\n", procA_pid);
		pid_a = atoi(procA_pid);
	}
	else
	{
		printf("proc a pid not f\n");
		pid_a = getppid();

	}

	initSharedMem();
	
	pid_t const bpid = getpid();
	
	pid_t c_pid = 0;
	if ((c_pid = fork()) < 0) {
		err_show("fork");
		delSHM();
		kill(pid_a, SIGTERM);
		return(1);
		/* значение errno будет установлено функцией fork() */
	} else if (c_pid == 0) { /* дочерний процесс */
		procC(bpid);
		exit(0);
	}

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	
	sa.sa_handler = &terminator_sig_hndlr;
	sigaction(SIGUSR1, &sa, NULL);
	sa.sa_handler = &terminator_sig_hndlr;
	sigaction(SIGTERM, &sa, NULL);
	sa.sa_handler = &terminator_sig_hndlr;
	sigaction(SIGINT, &sa, NULL);

	setvbuf(stdin, NULL, _IONBF, 0);

	setvbuf(stdout, NULL, _IONBF, 0);
	int i;
	char line[MAXLINE] = "";

	fd_set rfds, rset;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO, &rfds);

	int n = -1;

	char * pgs = 0;
	bool haveResultNumToWrite = 0;

	unsigned long int isqr = -1;
	while (!terminate_flag) {
		
		/* Wait up to five seconds. */
		int const timeInSec = (haveResultNumToWrite ? 1: 5);
		tv.tv_sec = timeInSec;
		tv.tv_usec = 0;
		errno = 0;
		rset = rfds;

		retval = select(STDIN_FILENO + 1, &rset, NULL, NULL, &tv);

		if (retval == -1) {
			if (EINTR == errno)
				continue;
			err_show("select()");
			continue;
		} else if (retval > 0)
			if (FD_ISSET(STDIN_FILENO, &rset))
				printf(" b:Data is available now. %d\n", retval);
			else {

				printf(" b:select Data is not available now.\n");
				continue;
			}
		else {
			//printf(" b%d", timeInSec);//No data within %d seconds.\n
			continue;
		}

		printf("  b:op N: %d\n", n);
		pgs = line;
			*line = '0';

		if (haveResultNumToWrite == 1) {
			printf(" b:!!write pending num square: %lu\n", isqr);

			if (0 != writeToShared(isqr)) {
				haveResultNumToWrite = 0;
				isqr = -1;
			}
		}

		errno = 0;
		//fgets(line, MAXLINE, stdin);
		int readn = read(STDIN_FILENO, line, MAXLINE - 1);
		if (readn > 0) {
			printf(" b:line recieved: '%s'\n", pgs);
			do {
				if (haveResultNumToWrite)
					printf(" b: transmitting number skipped : %lu. Reason: c was busy\n", isqr);
				i = atoi(pgs);
				printf(" b:num recieved: %d\n", i);
				isqr = i*i;

				haveResultNumToWrite = 1;

				printf(" b:num square: %lu\n", isqr);

				if (0 != writeToShared(isqr))
					continue;

				haveResultNumToWrite = 0;
			} while ((pgs = strchr(pgs, '\n')) 
				&& (++pgs, (pgs < (line + readn))) && !terminate_flag);

		} else if (readn < 0)
			err_show(" read");
		else {
			printf(" STDIN eof  zero \n");
			break;
		}

		++n;
	};
	printf(" process b exit\n");
	
	
	kill(c_pid, SIGTERM);
	wait(NULL);
	delSHM();
	kill(pid_a, SIGTERM);

	return 0;
}

