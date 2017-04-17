/*
 * main_b.c
 * 
 * Copyright 2017 MML <mml@mml-N551JX>
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
#include  <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <wchar.h>

enum {
	BUFF_SIZE = 128,
	MAXLINE = 128
};

#include <sys/shm.h>
#define ARRAY_SIZE  40000   
#define MALLOC_SIZE 100000  
#define SHM_SIZE    100000
#define SHM_MODE    0600 /* чтение и запись для владельца */

void err_sys(char const* str)
{
	puts(str);
}

void err_show(char const* str)
{
	printf("error: %s . erro = %d\n", str, errno);
}

void err_showE(char const* str)
{
	printf("error: %s . erro = %d\n", str, errno);
	exit(1);
}
char array[ARRAY_SIZE]; /* неинициализированные данные = bss */

int mainSH(void)
{
	int shmid;
	char *ptr, *shmptr;
	printf("array[] от %p до %p\n", (void *) &array[0], (void*) &array[ARRAY_SIZE]);
	//	678
	//	Глава 15. Межпроцессное взаимодействие
	printf("стек примерно %p\n", (void *) &shmid);
	if ((ptr = malloc(MALLOC_SIZE)) == NULL)
		err_sys("ошибка вызова функции malloc");
	printf("динамически выделенная область от %p до %p\n",
		(void *) ptr, (void *) ptr + MALLOC_SIZE);
	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)
		err_sys("ошибка вызова функции shmget");
	if ((shmptr = shmat(shmid, 0, 0)) == (void *) - 1)
		err_sys("ошибка вызова функции shmat");
	printf("сегмент разделяемой памяти присоединен в адресах от %p до %p\n",
		(void *) shmptr, (void *) shmptr + SHM_SIZE);
	if (shmctl(shmid, IPC_RMID, 0) < 0)
		err_sys("ошибка вызова функции shmctl");
	return(0);
}
typedef unsigned char bool;
bool const true = 1;
const bool false = 0;

typedef struct _TShMem {
	bool flag;
	pthread_mutex_t f_lock;
	long unsigned val;
} TShMem;

enum {
	MALLOC_SIZE2 = sizeof(TShMem)
};
int shmid = 0;
TShMem * ptrShMem = 0;
//char *;ptrShMem

void delSHM()
{
	if (0 != shmid) {
	
		pthread_mutex_destroy(&ptrShMem->f_lock);

		if (shmctl(shmid, IPC_RMID, 0) < 0)
			err_sys("ошибка вызова функции shmctl");
		else
			shmid = 0;
	}
}

int mainSH2(void)
{


	errno = 0;
	if ((shmid = shmget(IPC_PRIVATE, MALLOC_SIZE2, IPC_CREAT | IPC_EXCL | 0666)) < 0)
		err_showE("ошибка вызова функции shmget");
	errno = 0;
	if ((ptrShMem = shmat(shmid, 0, 0)) == (void *) - 1)
		err_showE("ошибка вызова функции shmat");
	printf("сегмент разделяемой памяти присоединен в адресах от %p до %p\n",
		(void *) ptrShMem, (void *) ptrShMem + MALLOC_SIZE2);
	//	EACCES		13	/* Permission denied */
	ptrShMem->flag = false;
	//PTHREAD_MUTEX_INITIALIZER

	if (pthread_mutex_init(&ptrShMem->f_lock, NULL) != 0) {
		err_sys("ошибка вызова функции pthread_mutex_init");
		return(1);
	}

	return(0);
}


//		int rtryl = pthread_mutex_trylock(&ptrShMem->f_lock);
//		if (rtryl == EBUSY)
int timedLockMut()
{
	int err;
	struct timespec tout;
	struct tm *tmp;
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	char buf[64];
	strftime(buf, sizeof(buf), "%r", tmp);
	wprintf(L"те­ку­щее вре­мя: %s\n", buf);
	tout.tv_sec += 5; /* 10 се­кунд, на­чи­ная от те­ку­ще­го вре­ме­ни */
	/* вни­ма­ние: это мо­жет при­вес­ти к ту­пи­ко­вой си­туа­ции */
	err = pthread_mutex_timedlock(&ptrShMem->f_lock, &tout);
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	wprintf(L"те­ку­щее вре­мя: %s\n", buf);
	if (err == 0)
		wprintf(L"мьютекс заперт!\n");
	else
	{
		wprintf(L"не по­лу­чи­лось по­втор­но за­пе­реть мью­текс: %d\n", err);
		if (ETIMEDOUT == err)
			wprintf(L"ETIMEDOUT");
		printf("\n");
			
	}
	return err;
}
int writeToShared(int isqr)
{
	int retc = timedLockMut();
	if (0 != retc)
		return retc;
		//pthread_mutex_lock(&fp->f_lock);
	if (ptrShMem->flag)
		printf("sh mem is busy! override !\n");
	
	

	ptrShMem->flag = true;
	ptrShMem->val = isqr;
	pthread_mutex_unlock(&ptrShMem->f_lock);
	return 0;
}

int main(int argc, char **argv)
{
	puts(__FILE__" Hello");

	if (argc > 0) {
		printf(" Hello, arg = %s\n", argv[0]);

	}
	mainSH2();
	pid_t pid = 0;
	if ((pid = fork()) < 0) {
		err_show("fork");
		return(1);
		/* значение errno будет установлено функцией fork() */
	} else if (pid == 0) { /* дочерний процесс */
		puts("proc C started!");

		do {
			//pthread_mutex_lock(&ptrShMem->f_lock);
			if (ptrShMem->flag) {
				printf("value = %ld\n", ptrShMem->val);
				ptrShMem->flag = false;
				fflush(stdout);
			} else
				sleep(1);
			//pthread_mutex_unlock(&ptrShMem->f_lock);
		} while (1);
		//        key_t ftok(const char *path, int id);
		//        Если нужно создать новую структуру IPC, а не получить ссылку на сущест
		//вующую, следует в  аргументе flag вместе с  флагом IPC_CREAT указать флаг
		//IPC_EXCL. В результате, если данная структура IPC уже существует, функция
		//вернет признак ошибки с кодом EEXIST.
		printf("!!exit c !!\n");
		exit(0);
	}
	atexit(delSHM);

	// setbuf(stdin, 0);
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
	do {
		/* Wait up to five seconds. */
		int const timeInSec = (haveResultNumToWrite ? 1: 5);
		tv.tv_sec = timeInSec;
		tv.tv_usec = 0;
		errno = 0;
		rset = rfds;

		retval = select(STDIN_FILENO + 1, &rset, NULL, NULL, &tv);
		/* Don’t rely on the value of tv now! */

		if (retval == -1)
			err_show("select()");
		else if (retval > 0)
			if (FD_ISSET(STDIN_FILENO, &rset))
				printf("Data is available now. %d\n", retval);
			else {

				printf("select Data is not available now.\n");
				continue;
			}
		else {
			printf("No data within %d seconds.\n", timeInSec);
			continue;
		}

		printf(__FILE__" op N: %d\n", n);
		pgs = line;
			*line = '0';

		if (haveResultNumToWrite == 1) {
			printf(" write pending num square: %lu\n", isqr);

			if (0 != writeToShared(isqr)) {
				haveResultNumToWrite = 0;
				isqr = -1;
			}
		}

		errno = 0;
		//fgets(line, MAXLINE, stdin);
		int readn = read(STDIN_FILENO, line, MAXLINE - 1);
		if (readn > 0) {
			printf(" line recieved: '%s'\n", pgs);
			i = atoi(pgs);
			printf(" num recieved: %d\n", i);
			isqr = i*i;
			haveResultNumToWrite = 1;
			
			printf(" num square: %lu\n", isqr);

			if (0 != writeToShared(isqr))
				continue;
			
			haveResultNumToWrite = 0;
			//vscanf("%d", &i);
		} else if ((readn < 0))//&&(errno != 0))
			err_show(" read");
		else {
			printf(" STDIN eof  zero \n");
		}
		//else
		++n;
	} while (n < 22); //((pgs != 0)&& ());
	printf(" process b exit\n");

	//write(STDOUT_FILENO, line, n); 
	//int i = 0;
	//scanf("%d", &i);
	return 0;
}

