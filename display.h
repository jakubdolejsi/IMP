
#include "MK60D10.h"
#include <stdbool.h>
//#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define SEGMENT_A 0x800
#define SEGMENT_B 0x200
#define SEGMENT_C 0x4000
#define SEGMENT_D 0x400
#define SEGMENT_E 0x40
#define SEGMENT_F 0x80
#define SEGMENT_G 0x8000
#define SEGMENT_DP 0x100

#define C1 0x100
#define C2 0x200
#define C3 0x1000
#define last 0x2000

#define D_OUTS 0xF300
#define A_OUTS 0xFC0

#define LEDS_START 7
#define LEDS_END 11

#define NUMBER_TIME 10000

#define PRINT_ZERO \
			PTA->PDOR |= SEGMENT_A;		\
			PTA->PDOR|= SEGMENT_F;		\
			PTA->PDOR |= SEGMENT_B;		\
			PTA->PDOR|= SEGMENT_E;		\
			PTD->PDOR |= SEGMENT_C;		\
			PTA->PDOR |= SEGMENT_D;		\



#define PRINT_ONE \
			PTA->PDOR |= SEGMENT_B; 	\
			PTD->PDOR |= SEGMENT_C; 	\

#define PRINT_TWO \
			PTA->PDOR |= SEGMENT_A; 	\
			PTA->PDOR |= SEGMENT_B; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTA->PDOR |= SEGMENT_E; 	\
			PTA->PDOR |= SEGMENT_D; 	\


#define PRINT_THREE \
			PTA->PDOR |= SEGMENT_A; 	\
			PTA->PDOR |= SEGMENT_B; 	\
			PTD->PDOR |= SEGMENT_C; 	\
			PTA->PDOR |= SEGMENT_D; 	\
			PTD->PDOR |= SEGMENT_G;		\


#define PRINT_FOUR \
				PTA->PDOR |= SEGMENT_F; 	\
			PTA->PDOR |= SEGMENT_B; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTD->PDOR |= SEGMENT_C; 	\

#define PRINT_FIVE \
				PTA->PDOR |= SEGMENT_F; 	\
			PTA->PDOR |= SEGMENT_A; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTD->PDOR |= SEGMENT_C; 	\
			PTA->PDOR |= SEGMENT_D;		\

#define PRINT_SIX \
				PTA->PDOR |= SEGMENT_F; 	\
			PTA->PDOR |= SEGMENT_A; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTD->PDOR |= SEGMENT_C; 	\
			PTA->PDOR |= SEGMENT_D;		\
			PTA->PDOR |= SEGMENT_E;		\

#define PRINT_SEVEN \
			PTA->PDOR |= SEGMENT_B; 	\
			PTA->PDOR |= SEGMENT_A; 	\
			PTD->PDOR |= SEGMENT_C; 	\

#define PRINT_EIGHT \
			PTA->PDOR |= SEGMENT_F; 	\
			PTA->PDOR |= SEGMENT_A; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTD->PDOR |= SEGMENT_C; 	\
			PTA->PDOR |= SEGMENT_D;		\
			PTA->PDOR |= SEGMENT_E;		\
			PTA->PDOR |= SEGMENT_B; 	\

#define PRINT_NINE \
			PTA->PDOR |= SEGMENT_F; 	\
			PTA->PDOR |= SEGMENT_A; 	\
			PTD->PDOR |= SEGMENT_G; 	\
			PTD->PDOR |= SEGMENT_C; 	\
			PTA->PDOR |= SEGMENT_D;		\
			PTA->PDOR |= SEGMENT_B; 	\



#define PRINT_FIRST_NUMBER(x) 	\
		PTD->PDOR &= ~C1; \
		PTD->PDOR |= C2; \
		PTD->PDOR |= C3; \
		PTD->PDOR |= last; \
		PTA->PDOR &= ~SEGMENT_DP; \
	switch(x){					\
		case 0:					\
			PRINT_ZERO					\
			break;						\
		case 1:						\
			PRINT_ONE					\
			break;						\
		case 2:							\
			PRINT_TWO					\
			break;						\
		case 3:							\
			PRINT_THREE					\
			break;						\
		case 4:							\
			PRINT_FOUR					\
			break;						\
		case 5:							\
			PRINT_FIVE					\
			break;						\
		case 6:							\
			PRINT_SIX					\
			break;						\
		case 7:							\
			PRINT_SEVEN					\
			break;						\
		case 8:							\
			PRINT_EIGHT					\
			break;						\
		case 9:							\
			PRINT_NINE					\
			break;						\
	} \


#define PRINT_SECOND_NUMBER(x) 	\
		PTD->PDOR |= C1; \
		PTD->PDOR |= C2; \
		PTD->PDOR |= C3; \
		PTD->PDOR &= ~last; \
		PTA->PDOR |= SEGMENT_DP; \
	switch(x){					\
		case 0:					\
			PRINT_ZERO					\
			break;						\
		case 1:						\
			PRINT_ONE					\
			break;						\
		case 2:							\
			PRINT_TWO					\
			break;						\
		case 3:							\
			PRINT_THREE					\
			break;						\
		case 4:							\
			PRINT_FOUR					\
			break;						\
		case 5:							\
			PRINT_FIVE					\
			break;						\
		case 6:							\
			PRINT_SIX					\
			break;						\
		case 7:							\
			PRINT_SEVEN					\
			break;						\
		case 8:							\
			PRINT_EIGHT					\
			break;						\
		case 9:							\
			PRINT_NINE					\
			break;						\
	}


#define PRINT_THIRD_NUMBER(x) 	\
		PTD->PDOR |= C1; \
		PTD->PDOR |= C2; \
		PTD->PDOR &= ~C3; \
		PTD->PDOR |= last; \
		PTA->PDOR &= ~SEGMENT_DP; \
	switch(x){					\
		case 0:					\
			PRINT_ZERO					\
			break;						\
		case 1:						\
			PRINT_ONE					\
			break;						\
		case 2:							\
			PRINT_TWO					\
			break;						\
		case 3:							\
			PRINT_THREE					\
			break;						\
		case 4:							\
			PRINT_FOUR					\
			break;						\
		case 5:							\
			PRINT_FIVE					\
			break;						\
		case 6:							\
			PRINT_SIX					\
			break;						\
		case 7:							\
			PRINT_SEVEN					\
			break;						\
		case 8:							\
			PRINT_EIGHT					\
			break;						\
		case 9:							\
			PRINT_NINE					\
			break;						\
	} \


#define PRINT_FOURTH_NUMBER(x) 	\
		PTD->PDOR |= C1; \
		PTD->PDOR &= ~C2; \
		PTD->PDOR |= C3; \
		PTD->PDOR |= last; \
		PTA->PDOR &= ~SEGMENT_DP; \
	switch(x){					\
		case 0:					\
			PRINT_ZERO					\
			break;						\
		case 1:						\
			PRINT_ONE					\
			break;						\
		case 2:							\
			PRINT_TWO					\
			break;						\
		case 3:							\
			PRINT_THREE					\
			break;						\
		case 4:							\
			PRINT_FOUR					\
			break;						\
		case 5:							\
			PRINT_FIVE					\
			break;						\
		case 6:							\
			PRINT_SIX					\
			break;						\
		case 7:							\
			PRINT_SEVEN					\
			break;						\
		case 8:							\
			PRINT_EIGHT					\
			break;						\
		case 9:							\
			PRINT_NINE					\
			break;						\
	}


#define ZERO_LEDS \
	PTA->PDOR &= ~SEGMENT_A; \
	PTA->PDOR &= ~SEGMENT_B; \
	PTA->PDOR &= ~SEGMENT_C; \
	PTA->PDOR &= ~SEGMENT_D; \
	PTA->PDOR &= ~SEGMENT_E; \
	PTA->PDOR &= ~SEGMENT_F; \
	PTD->PDOR &= ~SEGMENT_G; \
	PTD->PDOR &= ~SEGMENT_DP;

