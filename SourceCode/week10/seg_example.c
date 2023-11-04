#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

    short buff;
    // if you want to read='O_RDONLY' write='O_WRONLY' read$write='O_RDWR'
    int dev = open("/dev/my_segment", O_RDWR);

    if (argc < 2 ) {
        printf("put arg 0x0000 or int\n");
        return -1;
    }

    if (dev == -1) {
        printf("Opening was not possible!");
        return -1;
    }
    printf("Opening was successful!\n");

    if (argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X')) {
        buff = (unsigned short)strtol(&argv[1][2], NULL, 16);
    }
    else {
        buff = (unsigned short)strtol(&argv[1][0], NULL, 10);
    }

    write(dev, &buff, 2);
    close(dev);
    return 0;
}