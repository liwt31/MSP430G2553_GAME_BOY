#include "hardware.h"
#include "io430.h"

/***********************************
Hardware abstract
**********************************/

/* MCU initialization
 * Parameters : None
 * Return Value: None
 */
void IniMcu(){

    //Pe : pins for input
    PeSEL&=~KEY_ALL;
    PeSEL2&=~KEY_ALL;
    PeDIR&=~KEY_ALL;
    PeREN|=KEY_ALL;
    PeOUT|=KEY_ALL;
    PeIES|=KEY_ALL;
    PeIFG&=~KEY_ALL;
    PeIE|=KEY_ALL;

    //Pf : pins for output
    PfSEL&=~(LCD_CS+LCD_SCLK+LCD_SID+LCD_RST);
    PfSEL2&=~(LCD_CS+LCD_SCLK+LCD_SID+LCD_RST);
    PfOUT&=~(LCD_CS+LCD_SCLK+LCD_SID+LCD_RST);
    PfDIR|=LCD_CS+LCD_SCLK+LCD_SID+LCD_RST;

    //initialize system clock. In order to accelerate communication with LCD, 12MHZ is adopted
    BCSCTL1=CALBC1_12MHZ;
    DCOCTL=CALDCO_12MHZ;

    //initialize timer to provide random seed
    BCSCTL3=LFXT1S_2;
    TA0CTL|=TACLR+TASSEL_1+MC_1;
    TA0CCR0=12000;

}

/* LCD reset function
 * Parameters : None
 * Return Value: None
 * clear any statuses of LCD
 */
void RstLcd(){
    PfOUT&=~LCD_RST;
    __delay_cycles(10000);      //if the delay is too small, may cause malfunctioning of LCD
    PfOUT|=LCD_RST;
    __delay_cycles(10000);
}

/* Pulse function
 * Parameters : None
 * Return Value: None
 * Provide a pulse for SCLK
 */
extern inline void Pulse(){
    PfOUT|=LCD_SCLK;
    PfOUT&=~LCD_SCLK;
}

/* Sending a byte function
 * Parameters : None
 * Return Value: None
 * send a byte by SID
 */
void SendByte(unsigned char cmd){
    int i;
    for(i=0;i<8;i++){
        if((cmd<<i)&BIT7){
            PfOUT|=LCD_SID;
        }else{
            PfOUT&=~LCD_SID;
        }
        Pulse();
    }
}

/* Sending a command by serial communication
 * Parameters : command in int(16 bits)
 * Return Value: None
 * sending a complete command (usually divided into 3 SendByte function)
 */
void Send(int cmd){
    unsigned char cmd1=cmd/256;  //parameter cmd is divided to 3 parts, low 8 bits as 2 parts are instruction
    cmd%=256;
    unsigned char cmd2=cmd&0xF0,cmd3=(cmd&0x0F)<<4;

    PfOUT|=LCD_CS;

    if(cmd1){   //send RS according to cmd1
        SendByte(0xFA);
    }else{
        SendByte(0xF8);
    }
    SendByte(cmd2);
    SendByte(cmd3);
    PfOUT&=~LCD_CS;
}

/* Initialize LCD
 * Parameters : None
 * Return Value: None
 * Reset and turn the display on
 */
void IniLcd(){
    RstLcd();
    Send(DSPLY_ON);
    __delay_cycles(10000);  //after dsply_on cmd, a delay is needed
}


/* Write a sentence on LCd
 * Parameters : string of the sentence(16 bytes), which row to write(0 to 3)
 * Return Value: None
 * the char pointer has to contain 16 bytes of data, or a overflow will occur
 */
void WriteSentence(char* s,unsigned char row){
    Send(BIS);
    __delay_cycles(10000);  //the time delay is necessary, or the first line would not display properly
    switch(row){    //send the address of the row wish to write
        case(0):{
            Send(0x80);
            break;
        }
        case(1):{
            Send(0x90);
            break;
        }
        case(2):{
            Send(0x88);
            break;
        }
        case(3):{
            Send(0x98);
            break;
        }
        default:return;
    }
    unsigned char i;
    for(i=0;i<16;i++){      //send each of the bytes of the string
        Send(0x100+s[i]);
    }
}

/* Write a shorter sentence on LCd
 * Parameters : string of the sentence,bytes of the sentence(0~16), which row to write(0 to 3)
 * Return Value: None
 */
void WriteShortSentence(char* s,unsigned char sz,unsigned char row){
    Send(BIS);
    __delay_cycles(10000);  //the time delay is necessary, or the first line would not display properly
    switch(row){
        case(0):{
            Send(0x80);
            break;
        }
        case(1):{
            Send(0x90);
            break;
        }
        case(2):{
            Send(0x88);
            break;
        }
        case(3):{
            Send(0x98);
            break;
        }
        default:return;
    }
    unsigned char i;
    for(i=0;i<sz;i++){
        Send(0x100+s[i]);
    }
}

/* Reset the graph mode of LCD
 * Parameters : None
 * Return Value: None
 * enter graph mode and clear GDRAM
 */
void RstGraph(){
    Send(EIS);
    Send(GRAPH_OFF);    //must turn off display while drawing
    int i,j;    //clear all contents in GDRAM
    for(i=0;i<0x10;i++){    //horizontal coordinate
        for(j=0;j<0x20;j++){    //vertical coordinate
            Send(0x80+j);   //row(vertical coordinate) is sent first
            Send(0x80+i);   //then column data
            Send(0x100);    //clear high 8 bits
            Send(0x100);    //and low 8 bits
        }
    }
    Send(GRAPH_ON);
}

/* Write(draw) a point in LCD
 * Parameters : position of the point wish to draw(X for row and Y for column), shape wish to draw
 * Return Value: None
 * shp contains the information of two pixels, which is necessary because in LCD the 2 pixels is stored in the same GDRAM y address
 */
void WritePoint(unsigned char X,unsigned char Y,unsigned char shp){
    const unsigned int *cmd1=BITSHP[shp/16],*cmd2=BITSHP[shp%16];
    unsigned char x,y;  //address of the GDRAM, handbook is needed for understanding the relationship
    if(X<4){
        y=Y/2;
        x=X*8;
    }else{
        y=Y/2+8;
        x=(X-4)*8;
    }
    Send(GRAPH_OFF);
    unsigned char i;
    for(i=0;i<8;i++){   //write 8 rows in the same column
        Send(0x80+x+i);
        Send(0x80+y);
        Send(cmd1[i]);
        Send(cmd2[i]);
    }
    Send(GRAPH_ON);
}
/***********************************
end of hardware abstract
**********************************/
