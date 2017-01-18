#ifndef BASIC_H_INCLUDED
#define BASIC_H_INCLUDED

/***********************************
basic components: a Point structure, definition of modes, global variable direction
**********************************/

/* Point structure, 2 d Point with parameters x and y
 */
struct Point{
    unsigned char x,y;
};

//basic operating system:different modes, cooperate with interruption
#define MAIN_MENU 0x00
#define MAIN_OUT 0x01   //preparing to quit main menu

#define GS_DS 0x02  //difficulty select for greedy snake
#define GS_GAME 0x03    //game for GREEDY SNAKE
#define GS_END 0x04     //the end of a single GREEDY SNAKE game

#define PB_LS  0x05 //level select for push box
#define PB_GAME  0x06   //game in push box
#define PB_END  0x07    //the end of a single PUSH BOX game

#define RC_PS 0x08  //reverse chess:player select to be updated

#define PAUSE 0x0F


typedef unsigned char Mode;
Mode g_mode;

/* 4 directions, note that for the convenience of comparison number 3 is left blank intentionally
 * 定义全局变量蛇前进方向，通过按键中断修改
 */
#define D_UP 1
#define D_DOWN 2
#define D_LEFT 4
#define D_RIGHT 5
#define D_UNDETERMINED 7

unsigned char g_drctn;

/***********************************
end of basi components
**********************************/

#endif // BASIC_H_INCLUDED
