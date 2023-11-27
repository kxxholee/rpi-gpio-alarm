#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;
char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
char seg_dnum[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10};

void init_keyboard();
void close_keyboard();
void get_key(char *)
void print_menu();

int main(int argc, char* argv[]) {      /** MAIN: */
    // variables
    const unsigned short DELAY_COUNT = 2000; //us
    int SegmentDisplay = 0;
    // Device Open
    int Segment = open("/dev/my_segment", O_RDWR);
    int Button  = open("/dev/my_button",  O_RDWR);

    // Exception Handling
    if (Segment == -1) {
        printf("7 Segment Device access failure!\n");
        return -1;
    }
    else printf("7 Segment Device access successful!\n");

    if (Button  == -1) {
        printf("Button Devices access failure!\n");
        return -1;
    }
    else printf("Button Devices access successful!\n");

    // Setup()
    init_keyboard();
    print_menu();

    // Loop()
    unsigned char ButtonInput[2] = {0}; // -> 2 * sizeof(char) == sizeof(short)
    unsigned char DisplayBuff[4] = {0};

    for (char command = -1; command != 'q'; get_key(&command)) {
        if      (command == 'u') SegmentDisplay = (SegmentDisplay >= 9999) ? 0 : SegmentDisplay += 1;
        else if (command == 'd') SegmentDisplay = (SegmentDisplay <= 0) ? 9999 : SegmentDisplay -= 1;
        else if (command == 'p') SegmentDisplay = 0;
        
        read(Button, &ButtonInput, sizeof(ButtonInput));
        ButtonInput[1] = (unsigned char)(0); // Overflow 방지한다고 넣어두긴 함 -> 추후 수정가능

        if      (ButtonInput[0] == 'u') SegmentDisplay = (SegmentDisplay >= 9999) ? 0 : SegmentDisplay += 1;
        else if (ButtonInput[0] == 'd') SegmentDisplay = (SegmentDisplay <= 0) ? 9999 : SegmentDisplay -= 1;

        DisplayBuff[0] = (seg_num[ SegmentDisplay / 1000        ]   << 4) | 0x01; // first bit
        DisplayBuff[1] = (seg_num[(SegmentDisplay % 1000) / 100 ]   << 4) | 0x02; // second bit
        DisplayBuff[2] = (seg_num[(SegmentDisplay % 100) / 10   ]   << 4) | 0x04; // third bit
        DisplayBuff[3] = (seg_num[ SegmentDisplay % 10          ]   << 4) | 0x08; // fourth bit

        for (unsigned char index; index < 4; index++) {
            write(Segment, &DisplayBuff[index], sizeof(unsigned char));
            usleep(DELAY_COUNT);
        }
    }

    close_keyboard();
    close(Segment);
    close(Button);

    return 0;
}   /** END: */

/**
* @brief 현재 터미널 설정을 저장하고 새로운 터미널 속성을 만들어 적용함
* 키보드 입력이 터미널에 문자로 출력되지 않는 특징을 지님
*/
void init_keyboard() { //capture terminal settings
    tcgetattr(STDIN_FILENO, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= ~ICANON;
    new_setting.c_lflag &= ~ECHO;
    new_setting.c_cc[VMIN] = 0;
    new_setting.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_setting);
}
/**
* @brief 저장해둔 터미널의 설정을 다시 터미널에 적용함
*/
void close_keyboard() { // apply previous terminal settings
    tcsetattr(0, TCSANOW, &init_setting);
}

/**
* @brief MODIFIED: Pointer(ref)를 통한 접근으로 수정
*/
void get_key(char *key) {
    if (read(STDIN_FILENO, key, 1) != 1) *key = -1;
}

/**
* @brief 꾸민 안내판
*/
void print_menu() {
    printf("\n");
    printf("\n***********MENU***********");
    printf("\n* [q] : Kill Program     *");
    printf("\n* [u] : Nummber Up       *");
    printf("\n* [d] : Number Down      *");
    printf("\n* [p] : Count Settings   *");
    printf("\n**************************");
    printf("\n");
}