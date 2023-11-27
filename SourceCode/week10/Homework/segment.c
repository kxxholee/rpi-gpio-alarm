#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;
char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
char seg_dnum[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10};

#define D1 0x01;
#define D2 0x02;
#define D3 0x04;
#define D4 0x08;

void init_keyboard() { //capture terminal settings
    tcgetattr(STDIN_FILENO, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= ~ICANON;
    new_setting.c_lflag &= ~ECHO;
    new_setting.c_cc[VMIN] = 0;
    new_setting.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard() { // apply previous terminal settings
    tcsetattr(0, TCSANOW, &init_setting);
}

char get_key() {
    char ch = -1;
    if (read(STDIN_FILENO, &ch, 1) != 1) ch = -1;
    return ch;
}

void print_menu() {

}

int main(int argc, char* argv[]) {

    int device = open("/dev/my_segment", O_RDWR);
    if (device == -1) {
        printf("Opening was not possible!\n");
        return -1;
    }
    printf("device opening successful!");

    init_keyboard();
    print_menu();
    

    return 0;
}