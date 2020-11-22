/*
 * functions.c
 *
 *  Created on: May 16, 2020
 *      Author: miki
 */
#include "functions.h"
#include "variables.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stm32f4xx_hal_gpio.h>
#define LCD_I2C_SLAVE_ADDRESS 0x7E 						//adresa i2c interfejsa za LCD
extern TIME time;
extern I2C_HandleTypeDef hi2c1; 						//stavi hi2c koji koristis


/*----------------------------------------------------------------------------------------------------------------------
 * Funkcija za PID kontrolu grejaca inkubatora, vraca vrednost greske koja se koristi za prepravku  vrednosti kasnjenja
 * okidanja TRIACA koji pali grejac
 * --------------------------------------------------------------------------------------------------------------------*/
int PID_control(int setpoint, float Kp, float Ki ,float Kd, float stvarna_temperatura )
{
			Time= millis();                //trenutno vreme
	        elapsedTime = (double)(Time - timePrev);        //izracunati vreme proslog vremena prethodnog racunanja

	        PID_greska = stvarna_temperatura-setpoint  ;                                // greska

	        PID_i += PID_greska * elapsedTime;                // integralni deo
	        PID_d = (PID_greska - PID_prethodna_greska)/elapsedTime;   // deo izvoda
	        if(PID_greska<0)
	             {PID_i=0;}
	        PID_vrednost = Kp*PID_greska + Ki*PID_i ;                //PID korekcija

	        PID_prethodna_greska = PID_greska;                                //sacuvati trenutnu gresku za sledecu iteraciju
	        timePrev = Time;                        //sacuvati trenutno vreme za sledecu iteraciju

	        if(PID_vrednost > 10000)
	        	    	{
	        	    	PID_vrednost = 10000; //maksimalna vrednost kontrole u slucaju vremena od 10ms
	        	    	};
	       if(PID_vrednost <0)
	        {
	        	PID_vrednost = 0; //maksimalna vrednost kontrole u slucaju vremena od 10ms
	       	};
	    return (volatile unsigned int) PID_vrednost;
}
/*-----------------------------------------------------------------------------------------------
 * Konrola snage grejaca preko delay funkcije koja odredjuje okidanje TRIACa
 * ----------------------------------------------------------------------------------------------*/
void kontrola_grejac(int PID_Greska)
{

	HAL_GPIO_WritePin(FiringPin_GPIO_Port, FiringPin_Pin, GPIO_PIN_SET);//HALGPIOPINSET
	delay_us(max_firing_delay-PID_Greska);//Ovaj delay kontrolise snagu grejaca
	HAL_GPIO_WritePin(FiringPin_GPIO_Port, FiringPin_Pin, GPIO_PIN_RESET); //HALGPIOPINRESET
	delay_us(100);
	//zero_croosing=0;//false
}
/*-----------------------------------------------------------------------------------------------------------------------
 * Funkcija koja vrsi ocitavanje temperature i vlaznosti vazduha putem i2c Bus-a i vraca dbl* na niz gde su vrednosti
 * Temperature i Relativne vlaznosti vazduha skladistene
 * -----------------------------------------------------------------------------------------------------------------------*/
double * DHT12_ocitavanje(int DHT12_address)
{


	data[0]=0x00;//pocetna adresa
	HAL_I2C_Master_Transmit(&hi2c1,DHT12_address,data,1,10);
	HAL_I2C_Master_Receive(&hi2c1,DHT12_address,&data[1],5,10);
	RH_int=data[1];
	RH_dec=data[2];
	T_int=data[3];
	T_dec=data[4];

	if(data[5]==(data[1]+data[2]+data[3]+data[4]))//checksum provera
	{

		sprintf(RH_val,"%2d.%1d",RH_int,RH_dec);
		sprintf(T_val,"%2d.%1d",T_int,T_dec);
		RHT_val[0]=atof(T_val);
		RHT_val[1]=atof(RH_val);
		return RHT_val;

	}
	else
	{
		return  NULL;
	}

}
/*---------------------------------------------------------------------------------
 * Funkcija koja vadi vreme preko i2c interfejsa i upisuje isti u struct time.
 *
 *--------------------------------------------------------------------------------*/
void getTimeDate_DS3231(int DS3231_I2C_address)
{
	uint8_t get_time[7]={0};

	HAL_I2C_Mem_Read(&hi2c1,DS3231_I2C_address,0x00,1,get_time,7,1000);

	time.seconds=hextodec(get_time[0]);
	time.minutes=hextodec(get_time[1]);
	time.hours=hextodec(get_time[2]);
	time.day=hextodec(get_time[3]);
	time.date=hextodec(get_time[4]);
	time.month=hextodec(get_time[5]);
	time.year=hextodec(get_time[6]);
}
/*---------------------------------------------------------------------------------
 * Funkcija koja setuje vreme na DS3231 RTC modulu preko i2c interfejsa
 *
 *--------------------------------------------------------------------------------*/
void setTime_DS3231(int DS3231_I2C_address,uint8_t sec, uint8_t min, uint8_t hours, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	set_time[0]=dectohex(sec);
	set_time[1]=dectohex(min);
	set_time[2]=dectohex(hours);
	set_time[3]=dectohex(dow);
	set_time[4]=dectohex(dom);
	set_time[5]=dectohex(month);
	set_time[6]=dectohex(year);
	HAL_I2C_Mem_Write(&hi2c1,DS3231_I2C_address,0x00,1,set_time,7,1000);
}
/*-----------------------------------------------------------------------------
 *Funkcija koja vrsi konverziju iz decimalnih brojeva u hex brojeve
 *-----------------------------------------------------------------------------*/
uint8_t dectohex(int val)
{
	return(uint8_t)((val/10*16)+(val%10));
}
/*-----------------------------------------------------------------------------
 * Funkcija koja vrsi konverziju iz hex brojeva u decimalne brojeve
 * ---------------------------------------------------------------------------*/
int hextodec(uint8_t val)
{
	return (int)((val/16*10)+(val%16));
}
/*-----------------------------------------------------------------------------
 * funkcija koja trazi adresu uredjaja na I2C magistrali i vraca vrednost adrese
 * ---------------------------------------------------------------------------*/
int find_I2C_deviceAddress(void)
{
	uint8_t i=0;
	for(i=0;i<255;i++)
	{
		if(HAL_I2C_IsDeviceReady(&hi2c1,i,1,10)==HAL_OK)
		{
			HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			return i;
			break;
		}
	}
	return -1;//greska
}
/*-----------------------------------------------------------------------------
 * funkcija koja salje komandu na LCD kontroler putem i2c BUS-a
 * ---------------------------------------------------------------------------*/
void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);      //4BIT HI
	data_l = ((cmd<<4)&0xf0); //4BIT LO
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1,LCD_I2C_SLAVE_ADDRESS,(uint8_t *) data_t, 4, 100);
}
/*-----------------------------------------------------------------------------
 * funkcija koja salje podatke na LCD kontroler putem i2c BUS-a
 * ---------------------------------------------------------------------------*/
void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  	//en=1, rs=0
	data_t[1] = data_u|0x09;  	//en=0, rs=0
	data_t[2] = data_l|0x0D;  	//en=1, rs=0
	data_t[3] = data_l|0x09;  	//en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1, LCD_I2C_SLAVE_ADDRESS,(uint8_t *) data_t, 4, 100);
}
/*-----------------------------------------------------------------------------
 * funkcija koja brise sve podatke i CGRAM-a LCD kontroler putem i2c BUS-a
 * ---------------------------------------------------------------------------*/
void lcd_clear (void)
{
	lcd_send_cmd(0x01);
	//lcd_send_cmd (0x80);
	/*for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
	*/
}
/*-----------------------------------------------------------------------------
 * funkcija koja setuje poziciju cursora na LCD ekranu
 * ---------------------------------------------------------------------------*/
void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}
/*-----------------------------------------------------------------------------
 * Inicijalizacija samog LCD-a
 * ---------------------------------------------------------------------------*/
void lcd_init (void)
{
	// 4 BIT inicijalizacija
	HAL_Delay(50);  // cekaj >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // cekaj >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // cekaj  >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mod
	HAL_Delay(10);

  // inicijalizacija displeja
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0 display,C=0 cursor, B=0 blink  ---> display OFF, blink OFF, cursor OFF
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor OFF ,blink OFF
}
/*-----------------------------------------------------------------------------
 * Funkcija koja salje ceo string
 * ---------------------------------------------------------------------------*/
void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void setpointChange(void)
{
};
/*-----------------------------------------------------------------------------
 * Funkcija koja kontrolise okretanje step motora i njegovu brzinu.
 * Brzinu okretanja odredjuje parametar int delay.
 *
 * ---------------------------------------------------------------------------*/
void stepperMotorControlFD(int delay)
{
	/*--------------------------------------------------------------------------
	 * Full Drive mod koraci
	 *  1 1 0 0
	 *  0 1 1 0
	 *  0 0 1 1
	 *  1 0 0 1
	 *-------------------------------------------------------------------------- */
	int count=0;
	//Rezolucija motora je 0.70175 po koraku, odnosno 513 koraka za 360 stepeni
	//Kako bi se ovaj motor okrenuo pun krug za 1 sekundu, potrebno je koristiti
	//delay od 2ms.
	while(count<513)
	{
	//Step 1
	HAL_GPIO_WritePin(StepperMotorPin1_GPIO_Port, StepperMotorPin1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin2_GPIO_Port, StepperMotorPin2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin3_GPIO_Port, StepperMotorPin3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin4_GPIO_Port, StepperMotorPin4_Pin, GPIO_PIN_RESET );
	delay_ms(delay);
	//Step 2
	HAL_GPIO_WritePin(StepperMotorPin1_GPIO_Port, StepperMotorPin1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin2_GPIO_Port, StepperMotorPin2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin3_GPIO_Port, StepperMotorPin3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin4_GPIO_Port, StepperMotorPin4_Pin, GPIO_PIN_RESET );
	delay_ms(delay);
	//Step 3
	HAL_GPIO_WritePin(StepperMotorPin1_GPIO_Port, StepperMotorPin1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin2_GPIO_Port, StepperMotorPin2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin3_GPIO_Port, StepperMotorPin3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin4_GPIO_Port, StepperMotorPin4_Pin, GPIO_PIN_SET );
	delay_ms(delay);
	//Step 4
	HAL_GPIO_WritePin(StepperMotorPin1_GPIO_Port, StepperMotorPin1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(StepperMotorPin2_GPIO_Port, StepperMotorPin2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin3_GPIO_Port, StepperMotorPin3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin4_GPIO_Port, StepperMotorPin4_Pin, GPIO_PIN_SET );
	delay_ms(delay);
	HAL_GPIO_WritePin(StepperMotorPin1_GPIO_Port, StepperMotorPin1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin2_GPIO_Port, StepperMotorPin2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin3_GPIO_Port, StepperMotorPin3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(StepperMotorPin4_GPIO_Port, StepperMotorPin4_Pin, GPIO_PIN_RESET );
	delay_ms(delay);
	count++;
	}

};
void show_time(void)
{
	char * strptr;
	sprintf(lcd_string,"Vreme:%02d:%02d:%02d",time.hours, time.minutes,time.seconds); //string koji ispisuje vreme
	delay_ms(50);
	lcd_put_cur(1,0);
	delay_ms(50);
	lcd_send_string(lcd_string);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
	  *(strptr++)=0;
	}
};
void show_date(void)
{
	char * strptr;
	sprintf(lcd_string,"Datum:%02d-%02d-20%02d",time.date, time.month,time.year);	//string koji ispisuje datum
	delay_ms(50);
	lcd_put_cur(0,0);
	delay_ms(50);
	lcd_send_string(lcd_string);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
	  *(strptr++)=0;
	}
};
void show_tempAndHumidity(void)
{
	char * strptr;
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		*(strptr++)=0;
	}

	delay_ms(30);
	lcd_put_cur(0, 0);
	delay_ms(30);
	sprintf(lcd_string,"T=%2.1f[degC]",temperatura);
	lcd_send_string(lcd_string);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		*(strptr++)=0;
	}
	delay_ms(30);
	lcd_put_cur(1, 0);
	delay_ms(30);
	sprintf(lcd_string,"RV=%2.1f[%%]",RelVlaz);
	lcd_send_string(lcd_string);
};
void show_lowHumWarning(void)
{
	char * strptr;
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		*(strptr++)=0;
	}
	lcd_clear();
	delay_ms(50);
	lcd_put_cur(0, 0);
	strcpy(lcd_string,"Niska vlaznost!");
	lcd_send_string(lcd_string);
	delay_ms(50);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		  *(strptr++)=0;
	}

};
void start_menu_1(void){
	char * strptr;
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			*(strptr++)=0;
		}
		lcd_clear();
		delay_ms(50);
		lcd_put_cur(0, 0);
		strcpy(lcd_string,"*Inkubator v1.0*");
		lcd_send_string(lcd_string);
		delay_ms(50);
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			  *(strptr++)=0;
		}
		lcd_put_cur(1, 0);
		strcpy(lcd_string,"*Miroslav M*");
		lcd_send_string(lcd_string);
		delay_ms(50);
		strptr=lcd_string;
};
void start_menu_2(void){
	char * strptr;
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			*(strptr++)=0;
		}
		lcd_clear();
		delay_ms(50);
		lcd_put_cur(0, 0);
		strcpy(lcd_string,"*Tel:0691311063*");
		lcd_send_string(lcd_string);
		delay_ms(50);
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			  *(strptr++)=0;
		}
};
void start_menu_3(void){
	char * strptr;
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			*(strptr++)=0;
		}
		lcd_clear();
		delay_ms(50);
		lcd_put_cur(0, 0);
		strcpy(lcd_string,"1.Start Inkubacij");
		lcd_send_string(lcd_string);
		delay_ms(50);
		strptr=lcd_string;
		while(*(strptr) !='\0')
		{
			  *(strptr++)=0;
		}
		lcd_put_cur(1, 0);
		strcpy(lcd_string,"2.Prikaz Temp");
		lcd_send_string(lcd_string);
		delay_ms(50);
		strptr=lcd_string;
};
void incub_menu_1(void)
{
	char * strptr;
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		*(strptr++)=0;
	}
	lcd_clear();
	delay_ms(50);
	lcd_put_cur(0, 0);
	strcpy(lcd_string,"*Inkubacija*");
	lcd_send_string(lcd_string);
	delay_ms(50);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
	  *(strptr++)=0;
	}
	lcd_put_cur(1, 0);
	strcpy(lcd_string,"*Zapoceta*");
	lcd_send_string(lcd_string);
	delay_ms(50);
	strptr=lcd_string;
};
void incub_menu_3(uint8_t days, uint8_t hours)
{
	char * strptr;
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		*(strptr++)=0;
	}
	lcd_clear();
	delay_ms(50);
	lcd_put_cur(0, 0);
	strcpy(lcd_string,"Vreme do kraja");
	lcd_send_string(lcd_string);
	delay_ms(50);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
	  *(strptr++)=0;
	}
	lcd_put_cur(1, 0);
	//remaining days display
	itoa(days,lcd_string,10);
	strptr=lcd_string;
	while(*(strptr) !='\0')
	{
		strptr++;
	}
	strcpy(strptr," Dana ");
	while(*(strptr) !='\0')
	{
		strptr++;
	}
	itoa(hours,strptr,10);
	while(*(strptr) !='\0')
	{
		strptr++;
	}
	strcpy(strptr," Sati");
	lcd_send_string(lcd_string);
	delay_ms(50);
	strptr=lcd_string;
};
bool debouncingFunct(ulong debounceVal)
{
	static ulong startDebounceTime=0;
	static ulong currentDebounceTime=0;
	static ulong dbncCnt=0;
	if(0==dbncCnt)
	{
		startDebounceTime=millis();
	}
	else
	{
		//do nothing
	}

	currentDebounceTime=millis();

	//Debouncing check
	if(currentDebounceTime>(startDebounceTime+debounceVal))
	{
		debouncingStat=true;
		dbncCnt=0;
	}
	else
	{
		debouncingStat=false;
		dbncCnt++;
	}

	return debouncingStat; //returns the status of completed debouncing



}
void switchHold(ulong HoldTime)
{
	ulong StarTime_swH=millis();
	ulong HoldTime_swH=HoldTime;
	if(0==((StarTime_swH+HoldTime_swH) % HoldTime))
		{

		}


}
/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance==TIM2) //check if the interrupt comes from TIM2
        {
    	 count++;
    		  if(0==HAL_GPIO_ReadPin(RedButton_GPIO_Port, RedButton_Pin))
    		  {
    			  redButtonPressedStat=false;
    		  }
    		  if(0==HAL_GPIO_ReadPin(GreenButton_GPIO_Port,GreenButton_Pin))
    		  {
    			  greenButtonPressedStat=false;
    		  }
        }
}*/



