#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define CCU_HDR_LENGTH 10

int main(int argc, char **argv){

    if(argc != 4){
        printf(" argc ==  %d ./a.out 1)input 2)output 3) ip-packet-length\n", argc);
        exit(0);
    }

    FILE *ip, *op;
    struct stat st;
    unsigned char *buffer = (unsigned char *)malloc(atoi(argv[3]) *  sizeof(char));
    ip = fopen(argv[1], "rb");
    op = fopen(argv[2], "w+");
    int packet_size = atoi(argv[3]);
    unsigned short temp[(packet_size - CCU_HDR_LENGTH) / 2];

    if(ip != NULL && op != NULL){
        stat(argv[1], &st);
        int line_count = st.st_size / atoi(argv[3]);
        for(int i=0; i<line_count; i++){
            fread(buffer, atoi(argv[3]), 1, ip);
	    memset(&temp, 0, sizeof temp);
	    memcpy(&temp, &buffer[CCU_HDR_LENGTH], packet_size - CCU_HDR_LENGTH);
            for(int j=0; j<(sizeof temp)/2; j++)
                fprintf(op, "%04d ", ntohs(temp[j]));
            fprintf(op, "\n");
        }
    }

    fclose(ip);
    fclose(op);
}
