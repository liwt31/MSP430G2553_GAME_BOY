
/***********************************
interrupt vector
**********************************/


#pragma vector=PORT1_VECTOR
__interrupt void port1_isr(){
    unsigned char ifg=PeIFG;

    if(ifg&KEY_UP){ //in every mode, change of direction is applicable
        g_drctn=D_UP;
    }else if(ifg&KEY_LEFT){
        g_drctn=D_LEFT;
    }else if(ifg&KEY_DOWN){
        g_drctn=D_DOWN;
    }else if(ifg&KEY_RIGHT){
        g_drctn=D_RIGHT;
    }

    if(ifg&KEY_START){  //in greedy snake, restart the game after Pause
        if(g_mode==PAUSE){
            g_mode=GS_GAME;
            LPM0_EXIT;
        }
    }

    switch(g_mode){ //different sub functions for different modes
        case(MAIN_MENU):{
            if(ifg&KEY_YES){    //game chosen
                g_mode=MAIN_OUT;
            }
            break;
        }
        case(PB_LS):{
            IntPbLs(ifg);
            break;
        }
        case(PB_GAME):{
            IntPbGm(ifg);
            break;
        }
        case(PB_END):{
            IntPbEnd(ifg);
            break;
        }
        case(GS_DS):{
            IntGsDs(ifg);
            break;
        }
        case(GS_GAME):{
            IntGsGm(ifg);
            break;
        }
        case(GS_END):{
            IntGsEnd(ifg);
            break;
        }
        default:{
            break;
        }
    }
    if(g_mode!=GS_GAME){    //eliminate the flitter
        __delay_cycles(10000);
        while(PeIN!=0xFF);
        __delay_cycles(10000);
    }

    PeIFG=0;
}
/***********************************
end of interrupt vector
**********************************/
