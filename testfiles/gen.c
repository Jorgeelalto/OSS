#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv) {

	long int size, i, fd;
	char t, data = 'a';
	char* out_file;
	char* buffer;
	//Exit if the number of arguments is too low
	if (argc != 5) {
		printf("Usage: gen [SIZE] [K/M] [OUTPUT FILE] [LETTER/NUMBER]\n");
		return -1;
	}
	//Get the arguments
	else {
		//Set the data
		data = argv[4][0];
		//Select whether the size is given in KB or MB
		t = argv[2][0];
		//Get the size from the string in the argv
		size = atoi(argv[1]);
	}

	//If M is specified in the arguments, then the size is in MB
	if (t == 'M') size = size * 1024;
	else t == 'K';

	printf("Generating a file with size %s%c filled with \'%c\' stored as %s \n",
		argv[1], t, data, argv[3]);

	//Allocate the memory of the buffer with the specified number of KB
	buffer = malloc(1024*size);
	//Fill the buffer with the specified character
        memset(buffer, argv[4][0], size*1024);
	//Open the file
	fd = open(argv[3], O_WRONLY|O_CREAT|O_TRUNC, 0600);
	//Write the buffer to the file
	write(fd, buffer, strlen(buffer));
	//And close
	close(fd);
	return 0;
}
