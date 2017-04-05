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
	int	shmid;
	char	*ptr, *shmptr;
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
	if ((shmptr = shmat(shmid, 0, 0)) == (void *) -1)
		err_sys("ошибка вызова функции shmat");
	printf("сегмент разделяемой памяти присоединен в адресах от %p до %p\n",
		(void *) shmptr, (void *) shmptr + SHM_SIZE);
	if (shmctl(shmid, IPC_RMID, 0) < 0)
		err_sys("ошибка вызова функции shmctl");
	return (0);
}
typedef unsigned char bool;
bool const true = 1;
const bool false = 0;

typedef struct _TShMem {
	bool flag;
	long unsigned val;
} TShMem;

enum {
	MALLOC_SIZE2 = sizeof(TShMem)
};
int shmid = 0;
	 TShMem	* ptrShMem= 0;	
	//char *;ptrShMem
void delSHM()
{
	if (0 != shmid) {

		if (shmctl(shmid, IPC_RMID, 0) < 0)
			err_sys("ошибка вызова функции shmctl");
		else
			shmid = 0;
	}
}

int mainSH2(void)
{


	errno = 0;
	if ((shmid = shmget(IPC_PRIVATE, MALLOC_SIZE2, IPC_CREAT|IPC_EXCL|0666)) < 0)
		err_showE("ошибка вызова функции shmget");
	errno = 0;
	if ((ptrShMem = shmat(shmid, 0, 0)) == (void *) - 1)
		err_showE("ошибка вызова функции shmat");
	printf("сегмент разделяемой памяти присоединен в адресах от %p до %p\n",
		(void *) ptrShMem, (void *) ptrShMem + MALLOC_SIZE2);
	ptrShMem->flag = false;
//	EACCES		13	/* Permission denied */
	return(0);
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
        return (1);
        /* значение errno будет установлено функцией fork() */
    } else if (pid == 0) { /* дочерний процесс */
		puts("proc C started!");
		
		do
		{
			if (ptrShMem->flag) {
				printf("value = %ld\n", ptrShMem->val);
				ptrShMem->flag = false;
				fflush(stdout);
			}
			else
				sleep(1);
		} while (1);
//        key_t ftok(const char *path, int id);
//        Если нужно создать новую структуру IPC, а не получить ссылку на сущест
//вующую, следует в  аргументе flag вместе с  флагом IPC_CREAT указать флаг
//IPC_EXCL. В результате, если данная структура IPC уже существует, функция
//вернет признак ошибки с кодом EEXIST.
        exit(0);
    }   
    atexit(delSHM);

    setbuf(stdin, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
    int i;
    char line[MAXLINE] = "";
    int n = -1;
	char * pgs = 0;
	do {
		// n = read(STDIN_FILENO, &i, sizeof(i));
		printf(__FILE__" op N: %d\n", n);
		errno = 0;
		pgs = fgets(line, MAXLINE, stdin);
		if (pgs != 0) {
			printf(" line recieved: '%s'\n", pgs);
			i = atoi(pgs);
            printf(" num recieved: %d\n", i);
            unsigned long int isqr = i*i;
            printf(" num square: %lu\n", isqr);
			if (ptrShMem->flag)
				printf("sh mem is busy! \n");
            ptrShMem->flag = true;
			ptrShMem->val = isqr;
            //vscanf("%d", &i);
        } else if (errno != 0)
            err_show(" fgets");
		++n
    } while (1);//((pgs != 0)&& ( < 5));
    //write(STDOUT_FILENO, line, n); 
    //int i = 0;
    //scanf("%d", &i);
    return 0;
}

