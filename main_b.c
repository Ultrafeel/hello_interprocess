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

enum {
	BUFF_SIZE = 128,
	MAXLINE = 128
};
int main(int argc, char **argv)
{
	puts(" Hello" );
	
	if (argc > 0){
	printf(" Hello, fd = %s", argv[1] );

	}
		
         int32_t i;   
	int n = -1;
        do
        {
             n = read(STDIN_FILENO, &i, sizeof(i));
             printf(" number recieved: %+"PRId32, i);
	}
        while(n != -1);
	//write(STDOUT_FILENO, line, n); 
	//int i = 0;
	//scanf("%d", &i);
	return 0;
}

