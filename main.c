#include "display.h"


/**
 *
 *Vysli signal, zacni pocitat, jakmile signal dojde, na prisluny port se dostane 1, vyvola se preruseni, v obsluze preruseni se zastavi pocitani,
 * z napocitanych ticku se sestavi vzdalenost, a ta se nasledne zobrazi na display
 */

/**
 * PIN MAPPING
 * ---------------------------------------------------------------
 * ULTRASOUND
 * ---------------------------------------------------------------
 * PART			 P1 PIN		     P1 PORT 			   MCU PORT
 *
 * VCC		-		02
 * GRND		-		49
 * Trig		-		35		-	 	PTA27		-		PTA27
 * Echo		-		37		-		PTA26		-		PTA26
 *
 * DISPLAY
 * ---------------------------------------------------------------
 *	 SINGLE SEGMENT SCHEMA
 *	   A
 *	 F   B
 *	   G
 *	 E   C
 *	   D  DP
 *
 * ---------------------------------------------------------------
 * SEGMENT PARTS	JP1 BLOCK	P1 PIN			  P1 PORT				 MCU PORT
 *
 * A		-		03		-		26		-		PTA11			-		PTA11 a
 * B		-		01		-		28		-		PTA9			-		PTA9 b
 * C		-		07		-		22		-		MCU_SPI2_MISO	-		PTD14 c
 * D		-		05		-		24		-		PTA10			-		PTA10 d
 * E		-		04		-		25		-		PTA6			-		PTA6 e
 * F		-		02		-		27		-		PTA7			-		PTA7 f
 * G		-		08		-		21		-		MCU_SPI2_CS1	-		PTD15 g
 * DP		-		06		-		23		-		PTA8			-		PTA8 dp
 *
 * CONTROL PARTS    JP1 BLOCK	P1 PIN			  P1 PORT				 MCU PORT
 *
 * C1		-		12		-		17		-		MCU_I2C0_SCL	-		PTD8
 * C2		-		09		-		20		-		MCU_SPI2_MOSI	-		PTD13
 * C3		-		10		-		19		-		MCU_SPI2_CLK	-		PTD12
 * C4		-		11		-		18		-		MCU_I2C0_SDA	-		PTD9
 */

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

#define CLOCK_RATE 100 * pow(10,3)
#define TRIGGER 0x8000000
#define ECHO 0x4000000
#define ZERO ~0
#define TO_USEC * ( 0.3) // ~ (1 / 3)


unsigned long long int ticks = 0;
bool can_continue = true;
int disp = 0;

/* A delay function */
void delay(long long bound)
{

  long long i;

  for(i = 0; i < bound; i++)
  {

  }
}

/* Display init - according to documentation we set all display port to GPIO mode on multiplexor */
void init_segments()
{
    PORTA->PCR[11] = PORT_PCR_MUX(0x01);
    PORTA->PCR[9] = PORT_PCR_MUX(0x01);
    PORTA->PCR[10] = PORT_PCR_MUX(0x01);
    PORTD->PCR[14] = PORT_PCR_MUX(0x01);
    PORTA->PCR[6] = PORT_PCR_MUX(0x01);
    PORTA->PCR[7] = PORT_PCR_MUX(0x01);
    PORTD->PCR[15] = PORT_PCR_MUX(0x01);
    PORTA->PCR[8] = PORT_PCR_MUX(0x01);

}

void set_interrupt()
{
    PORTA->PCR[26] = ( PORT_PCR_ISF(0x01) /* Nuluj ISF (Interrupt Status Flag) */
    				| PORT_PCR_IRQC(0x0A) /* Interrupt enable on failing edge */
    				| PORT_PCR_MUX(0x01) /* Pin Mux Control to GPIO */
    				| PORT_PCR_PE(0x01) /* Pull resistor enable... */
    				| PORT_PCR_PS(0x01)); /* ...select Pull-Up */


}

void init_mcu()
{
	/* Initialize the MCU - basic clock settings, turning the watchdog off */
    MCG_C4       |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1  |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

    /* Turn on PORTA clock */
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;
}

/* Display init - according to documentation we set all display port to GPIO mode on multiplexor */
void init_control_leds()
{
	PORTD->PCR[8] = PORT_PCR_MUX(0x01);
	PORTD->PCR[13] = PORT_PCR_MUX(0x01);
	PORTD->PCR[12] = PORT_PCR_MUX(0x01);
	PORTD->PCR[9] = PORT_PCR_MUX(0x01);
}

/* Set port directions and zero trigger port*/
void set_direction()
{
    PORTA->PCR[27] = PORT_PCR_MUX(0x01);

	PTA->PDDR = GPIO_PDDR_PDD( TRIGGER | A_OUTS);

	PTD->PDDR = GPIO_PDDR_PDD( D_OUTS );
	PTA->PDOR &= ~TRIGGER;
}

/* Enable interrupt on port A*/
void PORTA_enable_interrupt()
{
	NVIC_ClearPendingIRQ(PORTA_IRQn); /* Zero interruption from port A*/
	NVIC_EnableIRQ(PORTA_IRQn);       /* Allow interruption on port A */
}

/* MCU and Ports initialize function */
void init_hardware(void)
{
	init_mcu();
    set_interrupt();
    init_segments();
    init_control_leds();
    set_direction();
    PORTA_enable_interrupt();
}




/* Interruption handler */
void PORTA_IRQHandler(void)
{
	// echo pin was activated, so ultrasound came back to Reciever
	if ((PORTA->ISFR & ECHO) && !(GPIOA_PDIR & ECHO))
	{
		can_continue = false;

		PORTA->ISFR = ZERO;

	}
}


/* Activate trigger port */
void send_trigger()
{
	PTA->PDOR |= TRIGGER;

	can_continue = true;
	delay(40);

	PTA->PDOR &= ~TRIGGER;
}


/* Timer */
void count()
{
	ticks = 0;
	do {
		ticks++;
	}
	while (can_continue);
}

/* Get distance*/
float compute()
{
	return (ticks TO_USEC / 58); // formula for distance computation
}

/* Begin transmiter part */
float transmit()
{
	send_trigger();
	count();

	return (compute());
}



/* Get certain digit from float result*/
int get_first_number(float res)
{
	return ((int)(floor( fabs( res) * 0.1 ) ) ) % 10;
}
int get_second_number(float res)
{
	return ((int)(floor( fabs( res) * 1 ) ) ) % 10;
}
int get_third_number(float res)
{
	return ((int)(floor( fabs( res) * 10 ) ) ) % 10;
}
int get_fourth_number(float res)
{
	return ((int)(floor( fabs( res) * 100 ) ) ) % 10;
}


/* Print result to display */
void print_result(float res)
{
	int first;
	int second;
	int third;
	int fourth;

	int can_cont = 0;

	if (res > 1000.0) {
		first = 1;
		second = 0;
		third = 0;
		fourth = 0;
	} else {
		first = get_first_number(res);
		second = get_second_number(res);
	    third= get_third_number(res);
	    fourth = get_fourth_number(res);
	}


    while(can_cont < NUMBER_TIME) {
		if(disp == 0) {
			ZERO_LEDS
			PRINT_FIRST_NUMBER(first)
			disp++;
			can_cont++;
			delay(300);
		} else if (disp == 1) {
			ZERO_LEDS
			PRINT_SECOND_NUMBER(second)
			disp++;
			can_cont++;
			delay(300);
		} else if (disp == 2) {
			ZERO_LEDS
			PRINT_THIRD_NUMBER(third)
			disp++;
			can_cont++;
			delay(300);
		} else if (disp == 3) {
			ZERO_LEDS
			PRINT_FOURTH_NUMBER(fourth)
			disp = 0;
			can_cont++;
			delay(300);
		}
    }
}

int main(void)
{
   init_hardware();

   while (1)
    {
    	print_result(transmit());
    }

   return 0;
}
