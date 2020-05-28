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
#define LCD_I2C_SLAVE_ADDRESS 0x7E 						//adresa i2c interfejsa za LCD
extern TIME time;
extern I2C_HandleTypeDef hi2c1; 						//stavi hi2c koji koristis


/*----------------------------------------------------------------------------------------------------------------------
 * Funkcija za PID kontrolu grejaca inkubatora, vraca vrednost greske koja se koristi za prepravku  vrednosti kasnjenja
 * okidanja TRIACA koji pali grejac
 * --------------------------------------------------------------------------------------------------------------------*/
int PID_control(int setpoint, float Kp, float Ki ,float Kd, float stvarna_temperatura )
{

	  if(PID_greska > 20)                              //Ki se koristi samo za vrednosti greske ispod 20degC
	    {
		  PID_i = 0;
	    };
	  	PID_greska = setpoint - stvarna_temperatura;

	    PID_p = Kp * PID_greska;                         //Racunanje P vrednosti
	    PID_i = PID_i + (Ki * PID_greska);               //Racunanje I vrednosti
	    timePrev = Time;                    			//Prethodno vreme
	    Time = millis();                    			//Trenutno ucitano vreme
	    elapsedTime = (Time - timePrev) / 1000.00;
	    PID_d = Kd*((PID_greska - PID_prethodna_greska)/elapsedTime);  //Racunanje D vrednosti
	    PID_vrednost = PID_p + PID_i + PID_d;                      //Racunanje cele PID vrednosti

	    if(PID_vrednost < 0)
	    	{
	    	PID_vrednost = 0; //
	    	};
	    if(PID_vrednost > 7400)
	    	{
	    	PID_vrednost = 7400; //maksimalna vrednost kontrole u slucaju vremena od 10ms
	    	};
	    PID_prethodna_greska = PID_greska; //prethodna greska.
	    return PID_vrednost;
}
/*-----------------------------------------------------------------------------------------------
 * Konrola snage grejaca preko delay funkcije koja odredjuje okidanje TRIACa
 * ----------------------------------------------------------------------------------------------*/
void kontrola_grejac(int PID_Greska)
{
		  delay_us(max_firing_delay-PID_Greska); //Ovaj delay kontrolise snagu grejaca
	      HAL_GPIO_WritePin(FiringPin_GPIO_Port, FiringPin_Pin, GPIO_PIN_SET);//HALGPIOPINSET
	      delay_us(100);
	      HAL_GPIO_WritePin(FiringPin_GPIO_Port, FiringPin_Pin, GPIO_PIN_RESET); //HALGPIOPINRESET
	      zero_croosing=0;//false
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

		sprintf(RH_val,"%2d.%2d",RH_int,RH_dec);
		sprintf(T_val,"%2d.%2d",T_int,T_dec);
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
	uint8_t get_time[7];

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
	int i=0;
	for(i=0;i<255;i++)
	{
		if(HAL_I2C_IsDeviceReady(&hi2c1,i,1,10)==HAL_OK)
		{
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
	//Rezolucija motora je 1.8deg po koraku, odnosno 200 koraka za 360 stepeni
	//Kako bi se ovaj motor okrenuo pun krug za 1 sekundu, potrebno je koristiti
	//delay od 5ms.
	while(count<50)
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
	count++;
	}

};



