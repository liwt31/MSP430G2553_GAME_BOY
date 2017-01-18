#ifndef HARDWARE_H_INCLUDED
#define HARDWARE_H_INCLUDED

/***********************************
Hardware abstract
**********************************/

/* MCU initialization
 * Parameters : None
 * Return Value: None
 */
//define the connection of pins
#define PeSEL P1SEL
#define PeSEL2 P1SEL2
#define PeOUT P1OUT
#define PeIN P1IN
#define PeDIR P1DIR
#define PeREN P1REN
#define PeIES P1IES
#define PeIFG P1IFG
#define PeIE P1IE

#define PfSEL P2SEL
#define PfSEL2 P2SEL2
#define PfOUT P2OUT
#define PfDIR P2DIR
#define PfREN P2REN
#define PfIES P2IES
#define PfIFG P2IFG
#define PfIE P2IE

#define LCD_RS BIT3
#define LCD_RW BIT4
#define LCD_E BIT5
#define LCD_RST BIT2
#define LCD_CS LCD_RS
#define LCD_SID LCD_RW
#define LCD_SCLK LCD_E

#define KEY_UP BIT0
#define KEY_LEFT BIT1
#define KEY_DOWN BIT2
#define KEY_RIGHT BIT3
#define KEY_YES BIT4
#define KEY_NO  BIT6
#define KEY_PAUSE BIT7
#define KEY_START BIT5

#define KEY_ALL (KEY_UP+KEY_LEFT+KEY_DOWN+KEY_RIGHT+KEY_YES+KEY_NO+KEY_PAUSE+KEY_START)

void IniMcu();

/* LCD reset function
 * Parameters : None
 * Return Value: None
 * clear any statuses of LCD
 */
void RstLcd();

/* Pulse function
 * Parameters : None
 * Return Value: None
 * Provide a pulse for SCLK
 */
extern inline void Pulse();

/* Sending a byte function
 * Parameters : None
 * Return Value: None
 * send a byte by SID
 */
void SendByte(unsigned char cmd);

/* Sending a command by serial communication
 * Parameters : command in int(16 bits)
 * Return Value: None
 * sending a complete command (usually divided into 3 SendByte function)
 */
//instruction set, sent by Send()
#define BIS 0x0030    //basic instruction set
#define EIS 0x0034    //extend instruction set
#define CLR_DSPLY 0x0001  //clear display in BIS
#define DSPLY_ON 0x000C   //enable display in BIS
#define CURSOR_ON 0x000F  //enable cursor in BIS (for diagnostic)
#define CURSOR_OFF 0x000C //disable cursor in BIS
#define GRAPH_ON 0x0036   //turn on graph in EIS
#define GRAPH_OFF 0x0034  //turn off graph in EIS (necessary while drawing)

void Send(int cmd);

/* Initialize LCD
 * Parameters : None
 * Return Value: None
 * Reset and turn the display on
 */
void IniLcd();

/* Write a sentence on LCd
 * Parameters : string of the sentence(16 bytes), which row to write(0 to 3)
 * Return Value: None
 * the char pointer has to contain 16 bytes of data, or a overflow will occur
 */
void WriteSentence(char* s,unsigned char row);

/* Write a shorter sentence on LCd
 * Parameters : string of the sentence,bytes of the sentence(0~16), which row to write(0 to 3)
 * Return Value: None
 */
void WriteShortSentence(char* s,unsigned char sz, unsigned char row);


/* Reset the graph mode of LCD
 * Parameters : None
 * Return Value: None
 * enter graph mode and clear GDRAM
 */
void RstGraph();

/* Write(draw) a point in LCD
 * Parameters : position of the point wish to draw(X for row and Y for column), shape wish to draw
 * Return Value: None
 * shp contains the information of two pixels, which is necessary because in LCD the 2 pixels is stored in the same GDRAM y address
 */
 //rank of different shape in BITSHP matrix
#define EMPTY 0x0   //empty pixel
#define BLOCK 0x1   //fill the pixel
#define FOOD 0x2    //food of the snake, something like a dot
#define DSTNY 0x2   // destination of box
#define WORKER 0x3     //man
#define TRI_UP 0x4  //head up triangle, represents the head of snake, the same herein after
#define TRI_DOWN 0x5    //head down
#define TRI_LEFT 0x6    //head left
#define TRI_RIGHT 0x7   //head right

#define TURN_UL 0x8     //turning body of snake connecting up and left pixels, the same hereinafter
#define TURN_LD 0x9     //left and down
#define TURN_DR 0xA     //down and right
#define TURN_RU 0xB     //right and up
#define BOX_OUT 0xC     // box in position
#define BOX_IN  0xD     // box out of position
#define BODY_UD 0xE     //vertical body of snake
#define BODY_LR 0xF     //horizontal body of snake

//vector for different shapes£¨lighted pixels in each row of the LCD are set to be 1)
const unsigned int BITSHP[16][8]={
{0x100,0x100,0x100,0x100,0x100,0x100,0x100,0x100},  //0 empty
{0x100,0x17E,0x17E,0x17E,0x17E,0x17E,0x17E,0x100},  //1 BLOCK
{0x100,0x118,0x13C,0x17E,0x17E,0x13C,0x118,0x100},   //2 food
{0x100,0x118,0x118,0x17E,0x118,0x13C,0x124,0x142},  //3 man
{0x100,0x100,0x118,0x118,0x13C,0x13C,0x17E,0x17E}, //4 tri_up
{0x17E,0x17E,0x13C,0x13C,0x118,0x118,0x100,0x100},  //5 tri_down
{0x100,0x103,0x10F,0x13F,0x13F,0x10F,0x103,0x100},  //6 tri_left
{0x100,0x1C0,0x1F0,0x1FC,0x1FC,0x1F0,0x1C0,0x100},  //7 tri_right

{0x17E,0x1FE,0x1FE,0x1FC,0x1F8,0x1F0,0x1E0,0x100},  //8 turn_ul
{0x100,0x1E0,0x1F8,0x1FC,0x1FC,0x1FE,0x1FE,0x1FE},  //9 turn ld
{0x100,0x107,0x11F,0x13F,0x13F,0x17F,0x17F,0x17F},   //A turn dr
{0x17E,0x17F,0x17F,0x13F,0x13F,0x11F,0x107,0x100},  //B turn ru
{0x1FF,0x1C3,0x1A5,0x199,0x199,0x1A5,0x1C3,0x1FF}, //C BOX_OUT box not in position
{0x1FF,0x181,0x199,0x1A5,0x1A5,0x199,0x181,0x1FF},  //D BOX_IN box in position
{0x17E,0x17E,0x17E,0x17E,0x17E,0x17E,0x17E,0x17E},  //E body_ud
{0x100,0x1FF,0x1FF,0x1FF,0x1FF,0x1FF,0x1FF,0x100}  //F body_lr
};

void WritePoint(unsigned char X,unsigned char Y,unsigned char shp);
/***********************************
end of hardware abstract
**********************************/


#endif // HARDWARE_H_INCLUDED
