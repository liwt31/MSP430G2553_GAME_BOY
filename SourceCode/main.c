/***********************************
expi_final
seperate file, 2 games.
**********************************/

#include "stdlib.h"
#include "in430.h"
#include "basic.h"
#include "hardware.c"
#include "push_box.c"
#include "greedy_snake.c"
#include "interrupt.c"


/* draw symbol for game selected in main function
 * parameter: index of the row to draw, such as 0 1 2, which correspond to the second third or fourth row in LCD
 * return value: none;
 */
void WriteSymbol(unsigned char line);

/***********************************
main function
**********************************/
int main( void )
{
    //hardware initialization

    WDTCTL = WDTPW + WDTHOLD;
    IniMcu();
    IniLcd();
    RstGraph();
    _EINT();

    while(1){
        Send(BIS);
        Send(CLR_DSPLY);
        WriteSentence("Select a game:  ",0);
        WriteSentence(" \25 GREEDY SNAKE ",1);
        WriteSentence("   PUSH BOX     ",2);
        WriteSentence("   REVERSE CHESS",3);
        g_mode=MAIN_MENU;
        g_drctn=D_UNDETERMINED;
        unsigned char game=0;
        while(g_mode==MAIN_MENU){   //while the game is not chose
            if(g_drctn==D_UNDETERMINED){    //wait for input
                continue;
            }
            switch(g_drctn){
                case(D_UP):{
                    if(game!=0){
                        WriteSymbol(--game);
                    }
                    break;
                }
                case(D_DOWN):{
                    if(game!=2){
                        WriteSymbol(++game);
                    }
                    break;
                }
            }
            g_drctn=D_UNDETERMINED;
        }
        Send(CLR_DSPLY);
        switch(game){   //switch mode according to the game chosen
            case(0):{
                g_mode=GS_DS;
                break;
            }
            case(1):{
                g_mode=PB_LS;
                break;
            }
            case(2):{
                g_mode=RC_PS;
                break;
            }
            default:{
                WriteSentence("game error",0);
                break;
            }
        }
        switch(g_mode){ //enter different sub functions according to different modes
            case(PB_LS):{
                PbMain();
                break;
            }
            case(GS_DS):{
                GsMain();
                break;
            }
            case(RC_PS):{   //the game has not developed yet
                break;
            }
        }
    }

    return 0;
}
/***********************************
end of main function
**********************************/


void WriteSymbol(unsigned char line){
    //clear the symbol area for 3 rows
    Send(0x88);
    Send(0x100+' ');
    Send(0x100+' ');
    Send(0x90);
    Send(0x100+' ');
    Send(0x100+' ');
    Send(0x98);
    Send(0x100+' ');
    Send(0x100+' ');

    switch(line){
        case(0):{   //draw each symbol (has to be two bytes,the first apace is necessary)
            Send(0x90);
            Send(0x100+' ');
            Send(0x115);
            break;
        }
        case(1):{
            Send(0x88);
            Send(0x100+' ');
            Send(0x115);
            break;
        }
        case(2):{
            Send(0x98);
            Send(0x100+' ');
            Send(0x115);
            break;
        }
    }
}


