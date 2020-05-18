/*
 * variables.h
 *
 *  Created on: May 16, 2020
 *      Author: miki
 */
#include  <stdint.h>
#include  <stdbool.h>
#ifndef INC_VARIABLES_H_
#define INC_VARIABLES_H_
#endif /*INC_VARIABLES_H_*/
/*
Note: #define VAR_DECLS 1 before including this file to DECLARE and INITIALIZE
global variables.  Include this file without defining VAR_DECLS to extern
these variables.
----------------------------------------------------------------------------*/

#ifndef VAR_DEFS          // Make sure this file is included only once
#define VAR_DEFS 1

/*----------------------------------------------
Setup variable declaration macros.
----------------------------------------------*/
#ifndef VAR_DECLS
# define _DECL extern
# define _INIT(x)
#else
# define _DECL
# define _INIT(x)  = x
#endif

/*----------------------------------------------
Declare variables as follows:

_DECL [standard variable declaration] _INIT(x);

where x is the value with which to initialize
the variable.  If there is no initial value for
the varialbe, it may be declared as follows:

_DECL [standard variable declaration];
----------------------------------------------*/

_DECL volatile unsigned long count _INIT(0);//count ms
_DECL int period _INIT(0);//period in ms
_DECL int period_cnt _INIT(0);//
_DECL int oldPeriod _INIT(0);//
_DECL int currentPeriod _INIT(0);
_DECL volatile unsigned long * timeValueptr _INIT(0);//ptr ka count ms
_DECL volatile unsigned long count_us _INIT(0); //count us
_DECL volatile unsigned long * timeValueptr_us _INIT(0);//ptr ka count us
_DECL volatile unsigned long currTime _INIT(0);
_DECL volatile unsigned long prevTime _INIT(0);
_DECL volatile unsigned long cnter _INIT(0);
_DECL float PID_p _INIT(0);
_DECL float PID_i _INIT(0);
_DECL float PID_d _INIT(0);
_DECL int PID_vrednost _INIT(0);
_DECL float PID_greska _INIT(0);
_DECL float PID_prethodna_greska _INIT(0);
_DECL float timePrev _INIT(0);
_DECL float Time _INIT(0);
_DECL float elapsedTime _INIT(0);
_DECL volatile bool  zero_croosing _INIT(0);
_DECL const int max_firing_delay _INIT(7400);
_DECL volatile uint8_t RH_int _INIT(0);
_DECL volatile uint8_t RH_dec _INIT(0);
_DECL volatile uint8_t T_int  _INIT(0);
_DECL volatile uint8_t T_dec  _INIT(0);
_DECL uint8_t data[6];
_DECL char T_val [6];
_DECL char RH_val [6];
_DECL double RHT_val[2];
_DECL uint8_t get_time[7];

#endif
//
