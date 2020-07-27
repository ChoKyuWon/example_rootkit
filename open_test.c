#include <stdio.h>
#include <fcntl.h>

int main(){
    int fd;
    fd = open("Makefile", O_RDONLY);
    if(fd == 0){
        printf("Open failed\n");
    }
    else{
        printf("open is success! fd is %d\n", fd);
        char buf[1024] = {0};
        read(fd, buf, 1024);
        printf("%s\n", buf);
    }
}