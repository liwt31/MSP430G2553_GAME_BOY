#ifndef GREEDY_SNAKE_H_INCLUDED
#define GREEDY_SNAKE_H_INCLUDED

/* snake structure
 * realized by a matrix of pixels
 */
struct Snake;

/* write shp matrix in Snake structure
 * Parameters: the pointer to the snake structure to be wrote, row and column of the position of the pixel wish to write, the shape of the pixel
 * Return Value: the value wrote in the shp matrix
 * because shp matrix records 8*16 pixels in a 8*8 matrix (compatible with LCD graph mode), a special function(this) is necessary to accomplish writing shp matrix
 */
unsigned char GsSetShp(struct Snake *S, unsigned char X, unsigned char Y, unsigned char PXshp);

/* Initialize snake structure
 * Parameters: the pointer to the snake structure
 * Return Value: None
 */
void GsIniSnake(struct Snake *S);

/* Grow of the snake
 * Parameters: pointer to the snake, pointer to the food Point
 * Return Value: discreet value indicating if the food is eaten or the game is over(1 food is eaten, 0xFF game over, 0 nothing happens)
 */

//decide which shape to adopt according to the positions of the 2 parts that the body connects
//the shapes include vertical horizontal and four turns, see WritePoint()
const unsigned char BODY_STATUS[6][6]={
    //row indicates facing to the tail, while column indicates facing to the head
//   0    U       D    0     L      R
    {0,0      ,0      ,0,      0,      0},  //0
    {0,BODY_UD,0      ,0,TURN_LD,TURN_DR},  //D
    {0,  0,    BODY_UD,0,TURN_UL,TURN_RU},  //U
    {0,0      ,0      ,0,      0,      0},  //0
    {0,TURN_RU,TURN_DR,0,BODY_LR,      0},  //R
    {0,TURN_UL,TURN_LD,0,      0,BODY_LR},  //L
};

unsigned char GsGrow(struct Snake *S,struct Point *food);

/* the move of snake
 * Parameters : pointer to the snake, pointer to the food Point
 * Return value: discreet value indicating if the food is eaten or the game is over(1 food is eaten, 0xFF game over, 0 nothing happens)
 */
unsigned char GsMove(struct Snake *S,struct Point *food);

/* update the food
 * Parameters : pointer to the snake, pointer to the food Point
 * Return value: None
 */
void UpdateFood(struct Snake *S,struct Point *food);

/* select difficulty of the game
 * Parameters : None
 * Return value: the difficulty of the game, defined by discreet numbers, the higher the more difficult
 * difficulty actually decides the time interval for the snake to move
 */
unsigned char GsSelectDffclty();

/* main interface of the game
 * Parameters : None
 * Return value: None
 */
void GsMain();

/* interrupt sub function in different mode
 * Parameters : copy of value in ifg register
 * Return value: None
 */
void IntGsDs(unsigned char ifg);    //difficulty select mode
void IntGsGm(unsigned char ifg);    //gaming mode
void IntGsEnd(unsigned char ifg);   //game ending mode
#endif // GREEDY_SNAKE_H_INCLUDED
