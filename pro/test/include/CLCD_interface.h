/*********************************************************************/
/*********************************************************************/
/**********************    Auther : abdo hamed     *******************/
/**********************    Layer  : HAL           *******************/
/**********************    SWC    :   DIO          *******************/
/**********************    Version: 1.00           *******************/
/*********************************************************************/
/*********************************************************************/


/*Preprocessor file guard*/
#ifndef CLCD_INTERFACE_H_
#define CLCD_INTERFACE_H_

void CLCD_voidSendCommand(u8 Copy_u8Command);
void CLCD_voidSendData(u8 Copy_u8Data);

void CLCD_voidInit(void);
void CLCD_voidSendstring(const char *Copy_pcString );

void CLCD_voidGoToXY(u8 Copy_u8XPos,u8 Copy_u8YPos);
char* itoa(int value, char* buffer, int base);
char* reverse(char *buffer, int i, int j);
void swap(char *x, char *y);
void CLCD_voidWriteNumber(u32 Copy_u32Number,u8 Copy_u8XPos,u8 Copy_u8YPos);

#endif
