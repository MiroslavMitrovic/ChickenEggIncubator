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
double * DHT12_ocitavanje(I2C_HandleTypeDef* i2c,int DHT12_address)
{


	data[0]=0x00;//pocetna adresa
	HAL_I2C_Master_Transmit(i2c,DHT12_address,data,1,10);
	HAL_I2C_Master_Receive(i2c,DHT12_address,&data[1],5,10);
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
		return (double*)-1;
	}

}
/*---------------------------------------------------------------------------------
 * Funkcija koja vadi vreme preko i2c interfejsa i upisuje isti u struct time.
 *
 *--------------------------------------------------------------------------------*/
void getTime_DS3231(I2C_HandleTypeDef* i2c,int DS3231_I2C_address)
{
	uint8_t get_time[7];

	HAL_I2C_Mem_Read(i2c,DS3231_I2C_address,0x00,1,get_time,7,1000);

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
void setTime_DS3231(I2C_HandleTypeDef* i2c,int DS3231_I2C_address,uint8_t sec, uint8_t min, uint8_t hours, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	set_time[0]=dectohex(sec);
	set_time[1]=dectohex(min);
	set_time[2]=dectohex(hours);
	set_time[3]=dectohex(dow);
	set_time[4]=dectohex(dom);
	set_time[5]=dectohex(month);
	set_time[6]=dectohex(year);
	HAL_I2C_Mem_Write(i2c,DS3231_I2C_address,0x00,1,set_time,7,1000);
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
int find_I2C_deviceAddress(I2C_HandleTypeDef* i2c)
{
	int i=0;
	for(i=0;i<255;i++)
	{
		if(HAL_I2C_IsDeviceReady(i2c,i,1,10)==HAL_OK)
		{
			return i;
			break;
		}
	}
	return -1;//greska
}






