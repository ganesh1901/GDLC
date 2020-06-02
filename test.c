#include <stdio.h>
#include <stdlib.h>
int  main(void)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("config.ini", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);
	
	int index = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("%s", line);
		
		line[read-1] = '\0';
		if(index == 2 || index == 3){
			printf(" atoi -- [%d] \n",  atoi(line));
		}
		index++;
	}

	free(line);
	exit(EXIT_SUCCESS);
}

