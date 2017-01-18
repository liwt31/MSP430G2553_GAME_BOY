#include "push_box.h"
/***********************************
realization of PUSH BOX. map struct and its 'member' function
**********************************/

/* the map of the game
 * realized by a matrix of "pixels"(actually 8*8)
 */
struct Pb_map{
    unsigned char shp[8][8];    //shapes of each pixels
    struct Point worker;     //position of the worker
    unsigned char shp_under_worker; //shp that the worker covers
    unsigned char goal;   //num of unoccupied destination
};

/* write shp matrix in map structure
 * Parameters: the pointer to the map structure to be wrote, row and column of the position of the pixel wish to write, the shape of the pixel
 * Return Value: the value wrote in the shp matrix
 * because shp matrix records 8*16 pixels in a 8*8 matrix (compatible with LCD graph mode), a special function(this) is necessary to accomplish writing shp matrix
 */
unsigned char PbSetShp(struct Pb_map *M, unsigned char X, unsigned char Y, unsigned char PXshp){
    // decide write at high 4 bits or low 4 bits in shp matrix according to the parity of Y
    if(Y%2){    //if Y is odd
        unsigned char temp=M->shp[X][Y/2]/16;
        return M->shp[X][Y/2]=temp*16+PXshp;    //write low 4 bits, high 4 remains
    }else{  //YÎªÅ¼Êý
        unsigned char temp=M->shp[X][Y/2]%16;
        return M->shp[X][Y/2]=PXshp*16+temp;
    }
}

/* get shp matrix in map structure
 * Parameters: the pointer to the map structure to be wrote, row and column of the position of the pixel wish to read
 * Return Value: contents of the shp
 */
unsigned char PbGetShp(const struct Pb_map *M, unsigned char X, unsigned char Y){
    //decide bits to read according to the parity of Y
    if(Y%2){    //if Y is odd
        return M->shp[X][Y/2]%16; //read low 4 bits
    }else{
        return M->shp[X][Y/2]/16;
    }
}

/* Initialize map structure
 * Parameters : the pointer to the map structure
 * Return Value : None
 */
void PbIniMap(struct Pb_map *M,unsigned char level){
    M->goal=0;
    int i,j;
    for(i=0;i<8;i++){   // set initial value of shp matrix according map data
        for(j=0;j<16;j++){
            unsigned char raw_shp=PB_MAP_DATA[level][i*4+j/4];  //map data is 4*8 unsigned char, so it needs unzip
            switch(j%4){    //take different 2 bits according to j
                case(0):{
                    raw_shp/=64;
                    break;
                }
                case(1):{
                    raw_shp/=16;
                    raw_shp%=4;
                    break;
                }
                case(2):{
                    raw_shp%=16;
                    raw_shp/=4;
                    break;
                }
                case(3):{
                    raw_shp%=4;
                    break;
                }
                default:{
                    WriteSentence("error j",0);
                    break;
                }
            }
            unsigned char shp;
            switch(raw_shp){    //convert the raw_shp to macro-defined ship
                case(0):{
                    shp=EMPTY;
                    break;
                }
                case(1):{
                    shp=BLOCK;
                    break;
                }
                case(2):{
                    shp=BOX_OUT;
                    break;
                }
                case(3):{
                    shp=DSTNY;
                    M->goal++;
                    break;
                }
                default:{
                    WriteSentence("error raw_shp",0);
                }
            }
            PbSetShp(M,i,j,shp);    //write the shp in its position
        }
    }

    M->worker.x=PB_MAP_DATA[level][32]/16;
    M->worker.y=PB_MAP_DATA[level][32]%16;
    M->shp_under_worker=EMPTY;  //assume worker would only born on empty space
    PbSetShp(M,M->worker.x,M->worker.y,WORKER);

    RstGraph();
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            WritePoint(i,j*2,M->shp[i][j]);
        }
    }
}

/* Write the sentence of level selection
 * Parameters : current level
 * Return Value : None
 * the function is called for 2 times currently so its defined
 */
void PbWriteLevel(unsigned char lvl){
        char cmd[16]="                ";
        cmd[8]='0'+lvl;
        if(lvl>0){
            cmd[6]='\21';
        }
        if(lvl<9){
            cmd[10]='\20';
        }
        WriteSentence(cmd,2);
}

/* Select level interface
 * Parameters : the level of the previous game
 * Return Value : selected level
 */
unsigned char PbSelectLevel(unsigned char prev_lvl){

    Send(GRAPH_OFF);
    Send(BIS);
    Send(CLR_DSPLY);

    WriteSentence("   PUSH   BOX   ",0);
    WriteSentence("Select level:   ",1);
    WriteSentence("                ",2);
    WriteSentence(" Y-begin N-back ",3);
    unsigned char lvl=prev_lvl;
    PbWriteLevel(lvl);
    g_drctn=D_UNDETERMINED;
    while(g_mode==PB_LS){   //while decision has not made
        while(g_mode==PB_LS&&g_drctn==D_UNDETERMINED);  //wait for input
        switch(g_drctn){
            case(D_RIGHT):{
                if(lvl<9){
                    lvl++;
                }
                break;
            }
            case(D_LEFT):{
                if(lvl>0){
                    lvl--;
                }
                break;
            }
            case(D_UP):case(D_DOWN):{
                break;
            }
            default:{
                WriteSentence("drctn error",0);
            }
        }
        g_drctn=D_UNDETERMINED;
        PbWriteLevel(lvl);  //change display according to the input
    }
    Send(CLR_DSPLY);
    return lvl;
}

/* the man tries to move
 * Parameters : the pointer to the map, two points: the destination, and the destination of destination
 * Return Value : selected level
 */
void PbPush(struct Pb_map* M,const struct Point p1,const struct Point p2){
    unsigned char X=M->worker.x,Y=M->worker.y;
    unsigned char X1=p1.x,Y1=p1.y,X2=p2.x,Y2=p2.y;
    switch(PbGetShp(M,X1,Y1)){  //according to different nature of destination
        case(EMPTY):{   //if its empty, just move
            WritePoint(X,Y,PbSetShp(M,X,Y,M->shp_under_worker)); //recover the previous man position
            WritePoint(X1,Y1,PbSetShp(M,X1,Y1,WORKER)); //write new man position
            M->worker.x=X1;
            M->worker.y=Y1;
            M->shp_under_worker=EMPTY;
            break;
        }
        case(BOX_OUT):{ //if its a box out of destination
            switch(PbGetShp(M,X2,Y2)){  //judge by the destination of destination
                case(EMPTY):case(DSTNY):{   //if its empty of destination, move
                    WritePoint(X,Y,PbSetShp(M,X,Y,M->shp_under_worker));
                    WritePoint(X1,Y1,PbSetShp(M,X1,Y1,WORKER));
                    if(PbGetShp(M,X2,Y2)==DSTNY){   //specifically, if its destination, unoccupied destination(goal) decreases
                        WritePoint(X2,Y2,PbSetShp(M,X2,Y2,BOX_IN));
                        M->goal--;
                    }else{
                        WritePoint(X2,Y2,PbSetShp(M,X2,Y2,BOX_OUT));
                    }
                    M->worker.x=X1;
                    M->worker.y=Y1;
                    M->shp_under_worker=EMPTY;
                    break;
                }
                case(BOX_OUT):case(BOX_IN):case(BLOCK):{    //if it's box or block, then nothing happens
                    break;
                }
            }
            break;
        }
        case(BOX_IN):{  //if its a box in the destination, similar to box out of destination
                switch(PbGetShp(M,X2,Y2)){
                case(EMPTY):case(DSTNY):{
                    WritePoint(X,Y,PbSetShp(M,X,Y,M->shp_under_worker));
                    WritePoint(X1,Y1,PbSetShp(M,X1,Y1,WORKER));
                    if(PbGetShp(M,X2,Y2)==DSTNY){
                        WritePoint(X2,Y2,PbSetShp(M,X2,Y2,BOX_IN));
                    }else{
                        WritePoint(X2,Y2,PbSetShp(M,X2,Y2,BOX_OUT));
                        M->goal++;
                    }
                    M->worker.x=X1;
                    M->worker.y=Y1;
                    M->shp_under_worker=DSTNY;
                    break;
                }
                case(BOX_OUT):case(BOX_IN):case(BLOCK):{
                    break;
                }
            }
            break;
        }
        case(BLOCK):{
            break;
        }
        case(DSTNY):{
            WritePoint(X,Y,PbSetShp(M,X,Y,M->shp_under_worker));
            WritePoint(X1,Y1,PbSetShp(M,X1,Y1,WORKER));
            M->worker.x=X1;
            M->worker.y=Y1;
            M->shp_under_worker=DSTNY;
            break;
        }
        default:{
            WriteSentence("shp error",0);
        }
    }
}

/* Main function of PUSH BOX
 * Parameters : None
 * Return Value : None
 */
void PbMain(){

    unsigned char lvl=0xFF;
    unsigned char prev_lvl=0;

    while(1){
        struct Pb_map pb;
        struct Pb_map* M=&pb;


        if(lvl==0xFF){
            lvl=PbSelectLevel(prev_lvl);
            prev_lvl=lvl;
        }
        if(g_mode==MAIN_MENU){
            return;
        }

        PbIniMap(M,lvl);
        g_drctn=D_UNDETERMINED;

        while(g_mode==PB_GAME){

            if(M->goal==0){
                g_mode=PB_END;
            }

            if(g_drctn==D_UNDETERMINED){
                continue;
            }
            unsigned char X=M->worker.x;
            unsigned char Y=M->worker.y;

            struct Point p1,p2;
            p1.x=p2.x=X;
            p1.y=p2.y=Y;
            switch(g_drctn){
                case(D_UP):{
                    p1.x-=1;
                    p2.x-=2;
                    break;
                }
                case(D_LEFT):{
                    p1.y-=1;
                    p2.y-=2;
                    break;
                }
                case(D_DOWN):{
                    p1.x+=1;
                    p2.x+=2;
                    break;
                }
                case(D_RIGHT):{
                    p1.y+=1;
                    p2.y+=2;
                    break;
                }
                default:{
                    WriteSentence("DIRECTION ERROR",0);
                }
            }
            PbPush(M,p1,p2);
            g_drctn=D_UNDETERMINED;
        }
        RstGraph();
        Send(GRAPH_OFF);
        Send(BIS);
        Send(CLR_DSPLY);
        if(g_mode==PB_LS){
            lvl=0xFF;
            continue;
        }
        WriteSentence("CONGRATULATIONS",0);
        WriteSentence("Press Y for next",1);
        WriteSentence("P - select level",2);
        WriteSentence("N - back to main",3);
        while(g_mode==PB_END);
        Send(CLR_DSPLY);
        switch(g_mode){
            case(PB_GAME):{
                if(lvl==9){
                    lvl=0;
                }else{
                    lvl++;
                }
                break;
            }
            case(MAIN_MENU):{
                return;
            }
            case(PB_LS):{
                lvl=0xFF;
                break;
            }
        }
    }
}

/* interrupt sub function in different mode
 * Parameters : copy of value in ifg register
 * Return value: None
 */

void IntPbLs(unsigned char ifg){    //when selecting level
    if(ifg&KEY_YES){    //press yes to enter the game
        g_mode=PB_GAME;
        return;
    }else if(ifg&KEY_NO){   // press no to quit to quit to main menu
        g_mode=MAIN_MENU;
        return;
    }
}

void IntPbGm(unsigned char ifg){ //when selecting level
    if(ifg&KEY_NO){ //press no to return to level select menu
        g_mode=PB_LS;
    }
}

void IntPbEnd(unsigned char ifg){   //when the game is over
    if(ifg&KEY_YES){    //press yes to play next level
        g_mode=PB_GAME;
    }else if(ifg&KEY_NO){   //press no to return to main menu
        g_mode=MAIN_MENU;
    }else if(ifg&KEY_PAUSE){    //press pause to return to level select interface
        g_mode=PB_LS;
    }
}


/***********************************
End of the realization of PUSH BOX
**********************************/
