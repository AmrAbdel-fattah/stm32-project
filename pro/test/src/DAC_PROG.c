/***************************************************************************************/
/****************************  IMT School Training Center ******************************/
/***************************************************************************************/
/** This file is developed by IMT School training center, All copyrights are reserved **/
/***************************************************************************************/

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "DIO_private.h"

#include "DAC_INTERFACE.h"
#include "song.h"

volatile u32 Song_Counter = 0;

void DAC_song( void ){
for (int i =0 ; i<2;i++){
	while (Song_Counter!=Song_Length){
	MGPIO_VidSetPinValue( GPIOA , 3 ,GET_BIT( Song_Array[ Song_Counter ],0) );
	MGPIO_VidSetPinValue( GPIOA , 4 ,GET_BIT( Song_Array[ Song_Counter ],1) );
	MGPIO_VidSetPinValue( GPIOA , 8 ,GET_BIT( Song_Array[ Song_Counter ],2) );
	MGPIO_VidSetPinValue( GPIOA , 9 ,GET_BIT( Song_Array[ Song_Counter ],3) );
	MGPIO_VidSetPinValue( GPIOA , 10 ,GET_BIT( Song_Array[ Song_Counter ],4) );
	MGPIO_VidSetPinValue( GPIOA , 11 ,GET_BIT( Song_Array[ Song_Counter ],5) );
	MGPIO_VidSetPinValue( GPIOA , 12,GET_BIT( Song_Array[ Song_Counter ],6) );
	MGPIO_VidSetPinValue( GPIOA , 13 ,GET_BIT( Song_Array[ Song_Counter ],7) );

	Song_Counter++;
	}

	if( Song_Counter == Song_Length ){
		Song_Counter = 0;
	}
}


}

void MDAC_VidInit( void ){


	MGPIO_VidSetPinDirection(GPIOA,3,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,4,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,8,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,9,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,10,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,11,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,12,0b0010);
	MGPIO_VidSetPinDirection(GPIOA,13,0b0010);



}
