/*
 * functions.h
 *
 *  Created on: May 16, 2020
 *      Author: miki
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_
#include "functions.h"
#include "variables.h"
#include "main.h"


unsigned long millis(void);
void delay_ms(unsigned long);
void delay_us(unsigned long);
int PID_control(int, float, float ,float, float);
void kontrola_grejac(int);
double* DHT12_ocitavanje(I2C_HandleTypeDef*,int);
void getTime_DS3231(I2C_HandleTypeDef*,int);
void setTime_DS3231(I2C_HandleTypeDef*,int,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
uint8_t dectohex(int val);
int hextodec(uint8_t val);
int find_I2C_deviceAddress(I2C_HandleTypeDef*);
void lcd_print(int,int);
/*Struct za pakovanje podataka sa eksternog RTC*/
typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t	year;
} TIME;
TIME time;

#endif /* INC_FUNCTIONS_H_ */
