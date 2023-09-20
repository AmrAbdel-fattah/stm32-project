
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "ir.h"
#include "RCC_interface.h"
#include "SPI_interface.h"

#include "EXTI0_interface.h"
#include "EXTI0_private.h"
#include "DIO_interface.h"
#include "NVIC_interface.h"
#include "STK_interface.h"

#include "TFT_interface.h"
#include "image.h"
#include "RCC_INTERFACE.h"
#include "DAC_INTERFACE.h"

#define SIZE 5

typedef struct nwPatient
{
	//u8 slot;
	u8 age;
	u8 gender;
	u8 id;
} patient;

patient nwRecord [SIZE];

/* Zero in slots means that the slot is reserved */
static u8 slots[6]  = {0,1, 2, 3, 4, 5};

/* Zero in slotLog means that the slot is not reserved */
static u8 slotLog [SIZE] = {0};

u8 addFlag = 0;

//void UI (void);
u8 addPatient  (void);
void editPatient (u8 idIn);
void reserveSlot (u8 idIn);
void cancelSlot  (u8 idIn);
void viewPatient (u8 idIn);
void viewResLog  (void);

/************************************************************************************/


void main(void)
{

	/**********************************************************************************/
	/* RCC Initialize */
	RCC_voidInitSysClock();
	RCC_voidEnableClock(RCC_APB2,2); /* GPIOA Enable Clock */
	RCC_voidEnableClock(RCC_APB2,3); /* GPIOB Enable Clock */
	//RCC_voidEnableClock(RCC_APB2,4); /* GPIOC Enable Clock */
	/*********************************************************/
	/*TFT*/
	RCC_voidEnableClock(RCC_APB2,12);  /* SPI1  */
	/*********************************************************/

	/* IR Initialization ,  IO Pins Initialization  */
	MGPIO_VidSetPinDirection(GPIOA,0,0b0100); /* A0 Input Floating */

	/* EXTI Initializtion */
	MEXTI_voidSetCallBack(voidGetFrame);
	MEXTI_voidInit(); /* A0 EXTI0 Enabled / Falling Edge */

	/* Enable EXTI0 from NVIC */
	MNVIC_voidInit();
	MNVIC_voidEnableInterrupt(6); /* EXTI0 */

	/* SysTick Init */
	MSTK_voidInit(); /* Enable SysTick AHB/8 = 1MHZ */
	/*********************************************************/

	/*TFT*/
	/* Pin Initializations */
	MGPIO_VidSetPinDirection(GPIOA,1,0b0010);  /* A0   output pushpull--> 2M*/
	MGPIO_VidSetPinDirection(GPIOA,2,0b0010);  /* RST   output pushpull--> 2M*/
	MGPIO_VidSetPinDirection(GPIOA,5,0b1001);  /* CLK   output AF pushpull--> 10M*/
	MGPIO_VidSetPinDirection(GPIOA,7,0b1001);  /* MOSI output AF pushpull--> 10M*/
	/************************************************************/




	/*LCD*/
	/* output pushpull--> 2M    0:7 pin for Data ,CLCD_RS PIN8, CLCD_RW_PIN9  ,CLCD_E PIN10*/
	MGPIO_VidSetPinDirection(GPIOB,0,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,1,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,2,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,3,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,4,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,5,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,6,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,7,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,8,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,9,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,10,0b0010);
	MGPIO_VidSetPinDirection(GPIOB,11,0b0010);
	/*****************************************************************************************/
	
	/**************************************/
/* DAC*/

   MDAC_VidInit();
/*
 
   DAC_song();
   
*/ 

	/*TFT*/
	/* SPI Init */
	MSPI1_voidInit();
	/* TFT Init */
	HTFT_voidInitialize();
	/*************************************************/

	/*********************************************************************************/
	u8 i;
	u8 feature = 0, featureUser = 0, id;
	u8 idIn;
	u32 mode = 0, adminPass, passIn = 1234, passInFlag = 0;

	while (mode <= 2)
	{
		CLCD_voidSendstring("There are two modes:" );
		CLCD_voidGoToXY(1,0);
		CLCD_voidSendstring("1.Admin. 2.User.");
		MSTK_voidSetBusyWait(4000000);
		CLCD_voidSendstring("enter number:");
		CLCD_voidGoToXY(1,0);
		/*waiting for user input*/
		while((GET_BIT(EXTI->PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
		/*	Clear pending Bit		*/
		SET_BIT(EXTI -> PR , 0);

		mode = voidTakeAction() ;

		switch (mode)
		{
		case 1:  passInFlag = 0;
		passIn = 1234;
		while (passInFlag < 3)
		{
			passInFlag++;
			CLCD_voidSendstring("Password: ");

			/*waiting for user input*/
			while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
			/*	Clear pending Bit*/
			SET_BIT(EXTI -> PR , 0);

			adminPass = voidTakeAction() ;


			CLCD_voidSendstring("passInFlag:");
			/*func to input string with passInFlag*/


			CLCD_voidSendstring("passIn:");
			/*func to input string with passIn*/


			CLCD_voidSendstring("adminPass:");
			/*func to input string with adminPass*/


			if (adminPass == passIn)
			{
				CLCD_voidSendstring("Feature:");
				/*func to input string with Feature*/


				feature = 0;
				while (feature <= 4)
				{
					/* Display Image Doctor */
					HTFT_voidDisplayImage(Doctor);
					CLCD_voidSendstring("1.Add record.");
					CLCD_voidGoToXY(1,0);
					CLCD_voidSendstring("2.Edit record.");
					MSTK_voidSetBusyWait(2000000);
					CLCD_voidSendstring("3.Reserve a slot ");
					CLCD_voidGoToXY(1,0);
					CLCD_voidSendstring("4.Cancel reservation");
					MSTK_voidSetBusyWait(2000000);
					CLCD_voidSendstring("Choose:");
					/*waiting for user input*/
					while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
					/*	Clear pending Bit		*/
					SET_BIT(EXTI -> PR , 0);

					feature = voidTakeAction() ;

					switch (feature)
					{
					case 1: id = addPatient  ();
					//CLCD_voidSendstring("New patient,ID = 1");
					/*func to input string with ID*/
					break;
					case 2: CLCD_voidSendstring("enter ID: ");

					/*waiting for user input*/
					while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
					/*	Clear pending Bit		*/
					SET_BIT(EXTI -> PR , 0);
					idIn = voidTakeAction() ;
					editPatient (idIn);
					break;
					case 3: CLCD_voidSendstring("enter ID: ");

					/*waiting for user input*/
					while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
					/*	Clear pending Bit		*/
					SET_BIT(EXTI -> PR , 0);

					idIn = voidTakeAction();
					reserveSlot(idIn);
					break;
					case 4: CLCD_voidSendstring("enter ID: ");

					/*waiting for user input*/
					while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
					/*	Clear pending Bit		*/
					SET_BIT(EXTI -> PR , 0);

					idIn = voidTakeAction();
					cancelSlot(idIn);
					break;
					default: CLCD_voidSendstring("Not a feature!");
					}
				}
				break;
			}
			else
			{ CLCD_voidSendstring("failed,again!"); }
		}
		break;
		case 2:
			featureUser = 0;
			while (featureUser <= 2)
			{
				/* Display Image Patient */
				HTFT_voidDisplayImage(Patient_t);



				CLCD_voidSendstring("1.View record");
				CLCD_voidGoToXY(1,0);
				CLCD_voidSendstring("2.View Slot.");
				MSTK_voidSetBusyWait(2000000);
				CLCD_voidSendstring("Choose:");

				/*waiting for user input*/
				while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
				/*	Clear pending Bit		*/
				SET_BIT(EXTI -> PR , 0);


				featureUser = voidTakeAction();

				switch (featureUser)
				{
				case 1: CLCD_voidSendstring("your ID: ");

				/*waiting for user input*/
				while((GET_BIT(EXTI -> PR,0)==0 )) {/*do nothing untill user enter anyhing*/}
				/*	Clear pending Bit		*/
				SET_BIT(EXTI -> PR , 0);

				idIn = voidTakeAction();
				viewPatient(idIn);
				break;
				case 2: viewResLog ();
				break;
				default: CLCD_voidSendstring("Not a feature");
				}
			}
			break;

		default: CLCD_voidSendstring("Unavailable!!");
		}
		CLCD_voidSendstring("Unavailable!!");
	}
	/*********************************************************************************/

	CLCD_voidSendstring("incorrect Choice");


}//MAIN



u8 addPatient(void)
{
	/*i  for index in for loop*/
	u8 i;
	addFlag++;

	/* Age */
	CLCD_voidSendstring("Enter age:");

	/*waiting for user input*/
	while((GET_BIT(EXTI -> PR ,0)==0 ))
	{
		/*do nothing untill user enter anyhing*/
	}
	/*	Clear pending Bit */
	SET_BIT(EXTI -> PR , 0);

	nwRecord[addFlag].age = voidTakeAction() ;

	/* Gender */
	CLCD_voidSendstring("Enter gender: ");

	/*waiting for user input*/
	while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
	/*	Clear pending Bit		*/
	SET_BIT(EXTI -> PR , 0);

	nwRecord[addFlag].gender = voidTakeAction();
	/* ID */
	nwRecord[addFlag].id = addFlag;

	for (i = 1; i <= addFlag; i++)
	{
		u8 temp,temp_1,temp_2;

		/*to display ID in LCD */
		temp_2=nwRecord[i].id;
		CLCD_voidSendstring("ID:");
		CLCD_voidWriteNumber(temp,0,4);

		/*to display gender in LCD */
		temp_1=nwRecord[i].gender;
		CLCD_voidSendstring("gender:");
		CLCD_voidWriteNumber(temp,1,8);

		/*to display age in LCD */
		temp=nwRecord[i].age;
		CLCD_voidGoToXY(0,5);
		CLCD_voidSendstring("age:");
		CLCD_voidWriteNumber(temp,0,10);

	}

	return nwRecord[addFlag].id;
}

void editPatient (u8 idIn)
{
	u8 edit, i,temp,temp_1,input;

	if (idIn != 0 && idIn <= addFlag)
	{
		temp = nwRecord[idIn].age;
		CLCD_voidSendstring("Age:");
		CLCD_voidWriteNumber(temp,0,4);
		temp_1= nwRecord[idIn].gender;
		CLCD_voidSendstring("Gender:");
		CLCD_voidWriteNumber(temp_1,1,6);
		MSTK_voidSetBusyWait(2000000);
		CLCD_voidSendstring("To edit,press 1");
		input=voidTakeAction();

		while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}

		SET_BIT(EXTI -> PR , 0);
		if (input ==1)
		{
			/* Age */
			CLCD_voidSendstring("edited age: ");
			/*waiting for user input*/
			while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
			/*	Clear pending Bit		*/
			SET_BIT(EXTI -> PR , 0);

			nwRecord[idIn].age = voidTakeAction();
			temp = nwRecord[idIn].age;
			CLCD_voidSendstring("Age:");
			CLCD_voidWriteNumber(temp,0,4);
			MSTK_voidSetBusyWait(2000000);
			/* Gender */
			CLCD_voidSendstring("edited gender: ");
			nwRecord[idIn].gender = voidTakeAction();
			temp = nwRecord[idIn].gender;
			/*waiting for user input*/
			while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
			/*	Clear pending Bit		*/
			SET_BIT(EXTI -> PR , 0);
			temp_1= nwRecord[idIn].gender;
			CLCD_voidSendstring("Gender:");
			CLCD_voidWriteNumber(temp_1,1,6);
			MSTK_voidSetBusyWait(2000000);

			nwRecord[addFlag].id = addFlag;

		}

	}
	else
	{ CLCD_voidSendstring("Incorrect ID!!"); }
}


void reserveSlot (u8 idIn)
{
	static u8 flag  = 0;
	static u8 choice;
	u8 i;

	if (idIn != 0 && idIn <= addFlag)
	{
		if (flag == 0)
		{
			CLCD_voidSendstring("Available Slots:" );
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendstring("1. 2pm:2:30pm ");
			MSTK_voidSetBusyWait(2000000);
			CLCD_voidSendstring("Available Slots:" );
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendstring("2. 2:30pm:3pm ");
			MSTK_voidSetBusyWait(2000000);
			CLCD_voidSendstring("Available Slots:" );
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendstring("3. 3pm:3:30pm ");
			MSTK_voidSetBusyWait(2000000);
			CLCD_voidSendstring("Available Slots:" );
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendstring("4. 4pm:4:30pm ");
			MSTK_voidSetBusyWait(2000000);
			CLCD_voidSendstring("Available Slots:" );
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendstring("5. 4:30pm:5pm ");

			CLCD_voidSendstring("Please choose slot number: ");

			/*waiting for user input*/
			while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
			/*	Clear pending Bit		*/
			SET_BIT(EXTI -> PR , 0);

			choice = voidTakeAction();

			if (choice >= 1 && choice <= 5)
			{
				slots[choice] = 0;
				slotLog[idIn] = choice;
				flag++;
			}
			else
			{ CLCD_voidSendstring("There are only 5 slots!"); }
		}

		/*****************
	 else
		{
			CLCD_voidSendstring("Available slots are:");

			for (i = 1; i <= 5; i++)
			{
				CLCD_voidSendstring("%d. %d\n", i, slots[i]);
			}

			CLCD_voidSendstring("Please choose slot number1: ");

			/*waiting for user input
			while((GET_BIT(EXTI -> PR ,0)==0 )) {/*do nothing untill user enter anyhing*/}
	/*	Clear pending Bit
			SET_BIT(EXTI -> PR , 0);


			choice = voidTakeAction();

			if (choice >= 1 && choice <= 5)
			{
				if (slots[choice] == 0)
				{ CLCD_voidSendstring("Slot is not available!!\n"); }
				else
				{
					slots[choice] = 0;
					slotLog[idIn] = choice;
				}
			}
			else
			{ CLCD_voidSendstring("There are only 5 slots!!\n"); }
		}
	} ***************************/
	else
	{ CLCD_voidSendstring("Incorrect ID!!\n"); }
}

void cancelSlot  (u8 idIn)
{
	u8 slotNum, i;

	if (idIn != 0 && idIn <= addFlag)
	{
		if (slotLog[idIn] == 0)
		{ CLCD_voidSendstring("No previous reservation!"); }
		else
		{
			slotNum = slotLog[idIn];
			slots[slotNum] = slotNum;
			slotLog[idIn] = 0;
		}
	}
	else
	{ CLCD_voidSendstring("Incorrect ID!"); }

}

void viewPatient (u8 idIn)
{
	u8 temp,temp_1,temp_2;

	/*to display ID in LCD */
	temp_2=nwRecord[idIn].id;
	CLCD_voidSendstring("ID:");
	CLCD_voidWriteNumber(temp,0,4);

	/*to display gender in LCD */
	temp_1=nwRecord[idIn].gender;
	CLCD_voidSendstring("gender:");
	CLCD_voidWriteNumber(temp,1,8);

	/*to display age in LCD */
	temp=nwRecord[idIn].age;
	CLCD_voidGoToXY(0,5);
	CLCD_voidSendstring("age:");
	CLCD_voidWriteNumber(temp,0,10);
}
/********************not complate ***************/
void viewResLog  (void)
{
	u8 i;

	for (i = 1; i <= addFlag; i++)
	{
		u8 temp;
		temp =slotLog[i];
		CLCD_voidSendstring("ID:");
		CLCD_voidWriteNumber(i,0,4);
		CLCD_voidSendstring("Slot:");;
		CLCD_voidWriteNumber(temp,1,6);
	}

}

