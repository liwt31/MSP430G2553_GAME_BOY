/***********************************
realization of greedy snake
**********************************/
#include "greedy_snake.h"

/* snake structure
 * realized by a matrix of pixels
 */
struct Snake{
    unsigned char dir[8][16];   //for each pixel of snake body, record the next position of snake body
    unsigned char shp[8][8];    //shape of each pixels
    struct Point head,tail;     //Position of the head and tail of snake
    unsigned char sz;   //the length of snake
};

/* write shp matrix in Snake structure
 * Parameters: the pointer to the snake structure to be wrote, row and column of the position of the pixel wish to write, the shape of the pixel
 * Return Value: the value wrote in the shp matrix
 * because shp matrix records 8*16 pixels in a 8*8 matrix (compatible with LCD graph mode), a special function(this) is necessary to accomplish writing shp matrix
 */
unsigned char GsSetShp(struct Snake *S, unsigned char X, unsigned char Y, unsigned char PXshp){
    // decide write at high 4 bits or low 4 bits in shp matrix according to the parity of Y
    if(Y%2){    //if Y is odd
        unsigned char temp=S->shp[X][Y/2]/16;
        return S->shp[X][Y/2]=temp*16+PXshp;    //write at low 4 bits while high 4 bits remains
    }else{  //if Y is even
        unsigned char temp=S->shp[X][Y/2]%16;
        return S->shp[X][Y/2]=PXshp*16+temp;
    }
}

/* Initialize snake structure
 * Parameters: the pointer to the snake structure
 * Return Value: None
 */
void GsIniSnake(struct Snake *S){
    int i,j;
    for(i=0;i<8;i++){   //clear dir value
        for(j=0;j<16;j++){
            S->dir[i][j]=EMPTY;
        }
    }
    for(i=0;i<8;i++){   //clear shp value
        for(j=0;j<8;j++){
            S->shp[i][j]=EMPTY;
        }
    }
    //the birth position of the snake is at row 4, column 7(center)
    S->head.x=S->tail.x=4;
    S->head.y=S->tail.y=7;
    S->dir[4][7]=1;
    S->sz=1;
    WritePoint(4,7,GsSetShp(S,4,7,BLOCK));
}

/* Grow of the snake
 * Parameters: pointer to the snake, pointer to the food Point
 * Return Value: discreet value indicating if the food is eaten or the game is over(1 food is eaten, 0xFF game over, 0 nothing happens)
 */

unsigned char GsGrow(struct Snake *S,struct Point *food){
    unsigned char hX=S->head.x;
    unsigned char hY=S->head.y;
    unsigned char localD=g_drctn;   //localize g_drctn to prevent g_drctn change while running this function
    if(abs(S->dir[hX][hY]-localD)==1){  //ignore the input if the direction is reverse of current direction (snake can't go this way)
        localD=S->dir[hX][hY];
    }
    WritePoint(hX,hY,GsSetShp(S,hX,hY,BODY_STATUS[S->dir[hX][hY]][localD]));    //convert current head to body, the shape is defined by the relative position of the 2 parts connected
    S->dir[hX][hY]=localD;

    switch(localD){
        case(D_UP):{
            if(hX==0){  //hit the wall and the game is over
                return 0xFF; //return game over
            }
            hX--;
            S->head.x--;
            WritePoint(hX,hY,GsSetShp(S,hX,hY,TRI_UP));  //new head
            break;
        }
        case(D_DOWN):{
            if(hX==7){
                return 0xFF;
            }
            hX++;
            S->head.x++;
            WritePoint(hX,hY,GsSetShp(S,hX,hY,TRI_DOWN));
            break;
        }
        case(D_LEFT):{
            if(hY==0){
                return 0xFF;
            }
            hY--;
            S->head.y--;
            WritePoint(hX,hY,GsSetShp(S,hX,hY,TRI_LEFT));
            break;
        }
        case(D_RIGHT):{
            if(hY==15){
                return 0xFF;
            }
            hY++;
            S->head.y++;
            WritePoint(hX,hY,GsSetShp(S,hX,hY,TRI_RIGHT));
            break;
        }
    }
    if(S->dir[hX][hY]){  //hit itself, game over
        return 0xFF;
    }else{  //or update direction
        S->dir[hX][hY]=localD;
    }
    S->sz++;
    return food->x==hX&&food->y==hY;    //return if the food is eaten
}

/* the move of snake
 * Parameters : pointer to the snake, pointer to the food Point
 * Return value: discreet value indicating if the food is eaten or the game is over(1 food is eaten, 0xFF game over, 0 nothing happens)
 */
unsigned char GsMove(struct Snake *S,struct Point *food){
    switch(GsGrow(S,food)){ //grow first, and according to the return value
        case(0):{   //nothing happens, clear the tail
            break;
        }
        case(1):{
            return 1;   //food eaten, return food is eaten, the tail remains
        }
        case(0xFF):{    //game over, return
            return 0xFF;
        }
        default:{
            Send(BIS);
            WriteSentence("Grow error",0);
        }
    }

    unsigned char tX=S->tail.x,tY=S->tail.y;
    WritePoint(tX,tY,GsSetShp(S,tX,tY,EMPTY));   //clear the tail
    switch(S->dir[tX][tY]){ //decide the new position of tail according to the direction the former tail recorded
        case(D_UP):{
            tX--;
            break;
        }
        case(D_DOWN):{
            tX++;
            break;
        }
        case(D_LEFT):{
            tY--;
            break;
        }
        case(D_RIGHT):{
            tY++;
            break;
        }
    }
    S->dir[S->tail.x][S->tail.y]=EMPTY;
    S->tail.x=tX;S->tail.y=tY;
    S->sz--;
    return 0;
}

/* update the food
 * Parameters : pointer to the snake, pointer to the food Point
 * Return value: None
 */
void UpdateFood(struct Snake *S,struct Point *food){
    unsigned char cnt=0,tcnt=1+rand()%(128-S->sz),i,j;  //randomly decide the position of the food
    for(i=0;i<8;i++){   //traverse the snake structure
        for(j=0;j<16;j++){
            if(S->dir[i][j]==0){    //if a vacant pixel is found
                cnt++;  //increase the count, until
                if(cnt==tcnt){  //the count is equal to target count decided randomly
                    food->x=i;  //update the coordinates of the food
                    food->y=j;
                    WritePoint(i,j,GsSetShp(S,i,j,FOOD));  //write the food
                    return;
                }
            }
        }
    }
}

/* select difficulty of the game
 * Parameters : None
 * Return value: the difficulty of the game, defined by discreet numbers, the higher the more difficult
 * difficulty actually decides the time interval for the snake to move
 */
unsigned char GsSelectDffclty(){
    Send(GRAPH_OFF);
    Send(BIS);
    Send(CLR_DSPLY);
    __delay_cycles(10000);
    //initialize the interface
    WriteSentence("  GREEDY SNAKE  ",0);
    WriteSentence("DIFFICULTY:     ",1);
    WriteSentence("        1 \20     ",2);
    WriteSentence(" Y-begin N-back ",3);
    //default difficulty is 1
    unsigned char dffclty=1;
    g_drctn=D_UNDETERMINED;
    while(g_mode==GS_DS){   //while the client has not decided which difficulty to play
        while(g_mode==GS_DS&&g_drctn==D_UNDETERMINED);  //if a input if found
        switch(g_drctn){
            case(D_RIGHT):{
                if(dffclty<5){      //change difficulty in the set range
                    dffclty++;
                }
                break;
            }
            case(D_LEFT):{
                if(dffclty>1){
                    dffclty--;
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
        char cmd[16]="                ";    //write the new difficulty
        cmd[8]='0'+dffclty;
        if(dffclty>1){
            cmd[6]='\21';
        }
        if(dffclty<5){
            cmd[10]='\20';
        }
        WriteSentence(cmd,2);
    }
    Send(CLR_DSPLY);
    __delay_cycles(10000);
    return dffclty;
}

/* main interface of the game
 * Parameters : None
 * Return value: None
 */
void GsMain(){

    //initialize the random number engine by timer
    srand(TA0R);

    unsigned long long delay_cycle=0;

    unsigned char score=0;

    while(1){

        if(delay_cycle==0){ //select difficulty first
            delay_cycle=0x2FFFF/GsSelectDffclty();
        }
        if(g_mode==MAIN_MENU){
            return;
        }

        while(g_mode==GS_GAME||g_mode==PAUSE){
            Send(GRAPH_ON);

                //randomly generate initial direction, note the 3 is blank so a special treatment is necessary
            g_drctn=1;
            do{
                g_drctn=1+rand()%5;
            }while(g_drctn==3);

            struct Snake s;
            struct Snake* S=&s;
            GsIniSnake(S);
            char hasFood=0;
            struct Point food;

            while(g_mode==GS_GAME||g_mode==PAUSE){
                // display the current score at the upright position
                score=S->sz;
                Send(BIS);
                Send(0x86);
                Send(0x100+'0'+score/100);
                Send(0x100+'0'+score%100/10);
                Send(0x100+'0'+score%10);
                Send(EIS);
                Send(GRAPH_ON);

                unsigned long long i;
                for(i=0;i<delay_cycle;i++){     //time interval for the snake to move
                    if(g_mode==PAUSE){  //allow client to pause the game
                        LPM0;
                    }
                }

                if(hasFood==0){ //if there is not any food, add one
                    UpdateFood(S,&food);
                    hasFood=1;
                }
                unsigned char result=GsMove(S,&food);   //move the snake
                if(result==1){     //if food is eaten, mark it
                    hasFood=0;
                }else if(result==0xFF){ //if hit itself, game over
                    g_mode=GS_END;
                }
            }
        }
        //the game process is terminated
        RstGraph();
        Send(GRAPH_OFF);
        Send(BIS);
        if(g_mode==GS_DS){  //if user decides to quit the game
            delay_cycle=0;  // return to difficulty select interface
            continue;
        }
        // if the game is over by hitting itself
        WriteShortSentence("YOUR SCORE:",11,0);
        WriteSentence("Press Y - replay",1);
        WriteSentence("P - select speed",2);
        WriteSentence("N - back to main",3);
        while(g_mode==GS_END);
        Send(CLR_DSPLY);
        __delay_cycles(10000);
        switch(g_mode){
            case(MAIN_MENU):{
                return;
            }
            case(GS_DS):{
                delay_cycle=0;
                break;
            }
            case(GS_GAME):{
                break;
            }
            default:{
                WriteSentence("gmode error",0);
            }
        }
    }
}

/* interrupt sub function in different mode
 * Parameters : copy of value in ifg register
 * Return value: None
 */

void IntGsDs(unsigned char ifg){    //difficulty select mode
    if(ifg&KEY_YES){    //enter the game
        g_mode=GS_GAME;
    }else if(ifg&KEY_NO){   //or quit to main menu
        g_mode=MAIN_MENU;
    }
}

void IntGsGm(unsigned char ifg){    //gaming mode
    if(ifg&KEY_NO){ //quit to difficulty select menu
        g_mode=GS_DS;
    }else if(ifg&KEY_PAUSE){    //pause
        g_mode=PAUSE;
    }
}

void IntGsEnd(unsigned char ifg){   //game ending mode
    if(ifg&KEY_YES){    //restart the game
        g_mode=GS_GAME;
    }else if(ifg&KEY_NO){   //quit to main menu
        g_mode=MAIN_MENU;
    }else if(ifg&KEY_PAUSE){    //or reselect difficulty
        g_mode=GS_DS;
    }
}

/***********************************
end of greedy snake
**********************************/
