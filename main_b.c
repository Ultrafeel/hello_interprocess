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
#define SHM_MODE    0600 /* чте­ние и за­пись для вла­дель­ца */


char array[ARRAY_SIZE]; /* не­ини­циа­ли­зи­ро­ван­ные дан­ные = bss */

int main2(void)
{
	int	shmid;
	char	*ptr, *shmptr;
	printf("array[] от %p до %p\n", (void *) &array[0], (void*) &array[ARRAY_SIZE]);
//	678
//	Глава 15. Межпроцессное взаимодействие
	printf("стек при­мер­но %p\n", (void *) &shmid);
	if ((ptr = malloc(MALLOC_SIZE)) == NULL)
		err_sys("ошиб­ка вы­зо­ва функ­ции malloc");
	printf("ди­на­ми­че­ски вы­де­лен­ная об­ласть от %p до %p\n",
		(void *) ptr, (void *) ptr + MALLOC_SIZE);
	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)
		err_sys("ошиб­ка вы­зо­ва функ­ции shmget");
	if ((shmptr = shmat(shmid, 0, 0)) == (void *) - 1)
		err_sys("ошиб­ка вы­зо­ва функ­ции shmat");
	printf("сег­мент раз­де­ляе­мой па­мя­ти при­со­еди­нен в ад­ре­сах от %p до %p\n",
		(void *) shmptr, (void *) shmptr + SHM_SIZE);
	if (shmctl(shmid, IPC_RMID, 0) < 0)
		err_sys("ошиб­ка вы­зо­ва функ­ции shmctl");
	return (0);
}
void err_show(char const* str)
{     
    printf("error: %s . erro = %d\n", str, errno);
}

int main(int argc, char **argv)
{
    puts(__FILE__" Hello");

    if (argc > 0) {
        printf(" Hello, arg = %s\n", argv[0]);

    }
    pid_t pid = 0l
    if ((pid = fork()) < 0) {
        err_show("fork");
        return (NULL);
        /* зна­че­ние errno бу­дет ус­та­нов­ле­но функ­ци­ей fork() */
    } else if (pid == 0) { /* до­чер­ний про­цесс */

//        key_t ftok(const char *path, int id);
//        Ес­ли нуж­но соз­дать но­вую струк­ту­ру IPC, а не по­лу­чить ссыл­ку на су­ще­ст­
//вую­щую, сле­ду­ет в  ар­гу­мен­те flag вме­сте с  фла­гом IPC_CREAT ука­зать флаг
//IPC_EXCL. В ре­зуль­та­те, ес­ли дан­ная струк­ту­ра IPC уже су­ще­ст­ву­ет, функ­ция
//вер­нет при­знак ошиб­ки с ко­дом EEXIST.
        exit(0);
    }   
    
    setbuf(stdin, 0);
    int i;
    char line[MAXLINE] = "";
    int n = -1;
    char * pgs = 0;
    do {
        // n = read(STDIN_FILENO, &i, sizeof(i));
        printf(__FILE__" op N: %d\n", n);
      pgs = fgets(line, MAXLINE, stdin);
        if (pgs != 0) {
            printf(" line recieved: '%s'\n", pgs);
            i = atoi(pgs);
            printf(" num recieved: %d\n", i);
            unsigned long int isqr = i*i;
            printf(" num square: %ul\n", isqr);
            
            //vscanf("%d", &i);
        } else
            err_show(" fgets");
    } while ((pgs != 0)&& (++n < 5));
    //write(STDOUT_FILENO, line, n); 
    //int i = 0;
    //scanf("%d", &i);
    return 0;
}

