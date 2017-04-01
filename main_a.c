
#include <stdio.h>
#include <stdlib.h>
enum {
	BUFF_SIZE = 128
};

int main()
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
