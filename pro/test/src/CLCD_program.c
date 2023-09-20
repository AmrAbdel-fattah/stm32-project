/*********************************************************/
/***********		Author: ABDO HAMED   	**************/
/***********		Layer: HAL			 	**************/
/***********		Component: CLCD 		**************/
/***********		Version: 1.00		 	**************/
/***********		Date: 14-8/2020		 	**************/
/*********************************************************/

#include "STD_TYPES.h"
//#include <util/delay.h>

#include "DIO_interface.h"
#include "STK_interface.h"

#include "CLCD_config.h"
#include "CLCD_interface.h"
#include "CLCD_private.h"

void CLCD_voidSendCommand(u8 Copy_u8Command)
{
	/*Set RS pin to low for command*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_RS_PIN,LOW);

	/*Set RW pin to low for write*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_RW_PIN,LOW);

	/*Set command to data pins*/
	/*HIGH 8 PINS*/
	MGPIO_VidSetPortValue(CLCD_DATA_PORT,HIGH,Copy_u8Command);
	/*LOW 8 PINS*/
	//MGPIO_VidSetPortValue(CLCD_DATA_PORT,LOW,Copy_u8Command);

	/*Send enable pulse*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,HIGH);
	MSTK_voidSetBusyWait(2000);
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,LOW);
}

void CLCD_voidSendData(u8 Copy_u8Data)
{
	/*Set RS pin to high for data*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_RS_PIN,HIGH);

	/*Set RW pin to low for write*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_RW_PIN,LOW);

	/*Set Data to data pins*/
	MGPIO_VidSetPortValue(CLCD_DATA_PORT,HIGH,Copy_u8Data);

	/*Send enable pulse*/
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,HIGH);
	MSTK_voidSetBusyWait(2000);
	MGPIO_VidSetPinValue(CLCD_CTRL_PORT,CLCD_E_PIN,LOW);
}

void CLCD_voidInit(void)
{
	/*Wait for more than 30 ms*/
	MSTK_voidSetBusyWait(40000);

	/*Function set command: 2 lines, 5*8 Font size*/
	CLCD_voidSendCommand(0b00111000);

	/*Display on off control: display enable, disable cursor, no blink cursor*/
	CLCD_voidSendCommand(0b00001100);

	/*Clear display*/
	CLCD_voidSendCommand(1);
}
void CLCD_voidSendstring(const char *Copy_pcString )
{
	/* Local loop index */
	u8 Local_u8Counter=0;
	while (Copy_pcString[Local_u8Counter] != '\0')
	{
		CLCD_voidSendData(Copy_pcString[Local_u8Counter]);

		Local_u8Counter++;
		MSTK_voidSetBusyWait(2000);
	}

}
void CLCD_voidGoToXY(u8 Copy_u8XPos,u8 Copy_u8YPos)
{
	u8 Local_u8Address;
	if(Copy_u8XPos==0)
	{
		Local_u8Address=Copy_u8YPos;
	}
	else if (Copy_u8XPos==1)
	{
		Local_u8Address=Copy_u8YPos+0x40;
	}
	CLCD_voidSendCommand(Local_u8Address+128);
}
void CLCD_voidWriteSpecialCharacter(u8* Copy_pu8Pattern,u8 Copy_u8PatternNumber,u8 Copy_u8XPos,u8 Copy_u8YPos)
{
	u8 Local_u8Index,Local_u8CGRAMAddress=0;

	/*calculate the CGRAM address whose each block is 8 bytes*/
	Local_u8CGRAMAddress=Copy_u8PatternNumber*8;

	/*senf CGRAM address command to LCD with setting bit 6 ,clearing bit 7*/
	CLCD_voidSendCommand(Local_u8CGRAMAddress+64);

	/*write the puttern into CGRAM*/
	for (Local_u8Index=0;Local_u8Index<8;Local_u8Index++)
	{
		CLCD_voidSendData (Copy_pu8Pattern[Local_u8Index]);
	}

	/*go back to the DDRAM to display the pattern*/
	CLCD_voidGoToXY(Copy_u8XPos,Copy_u8YPos);

	/*display the pattern written in the CGRAM*/
	CLCD_voidSendData(Copy_u8PatternNumber);
}

void CLCD_voidWriteNumber(u32 Copy_u32Number,u8 Copy_u8XPos,u8 Copy_u8YPos)
{
	//Copy_u32Number=999;
	u8 Local_u8Index;//Local_u8Check;
	CLCD_voidGoToXY(Copy_u8XPos,Copy_u8YPos);
	u8 Copy_u32Character[3]={' '};
	for (Local_u8Index=0;Local_u8Index<3;Local_u8Index++)
	{
		Copy_u32Character[Local_u8Index]=' ';
	}

	itoa (Copy_u32Number, Copy_u32Character,10);

	for (Local_u8Index=0;Local_u8Index<3;Local_u8Index++)
	{
		if ((Copy_u32Character[Local_u8Index]<'0')||(Copy_u32Character[Local_u8Index]>'9'))
		{
			CLCD_voidSendData(' ');
		}
		else
		{
			CLCD_voidSendData(Copy_u32Character[Local_u8Index]);
		}
		Copy_u32Number+=1;
		if (Copy_u32Number>999)
		{
			Copy_u32Number=0;
		}
	}

}
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}
// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32) {
		return buffer;
	}

	// consider the absolute value of the number
	u8  n = value;

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) {
			buffer[i++] = 65 + (r - 10);
		}
		else {
			buffer[i++] = 48 + r;
		}

		n = n / base;
	}

	// if the number is 0
	if (i == 0) {
		buffer[i++] = '0';
	}

	// If the base is 10 and the value is negative, the resulting string
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10) {
		buffer[i++] = '-';
	}

	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}
