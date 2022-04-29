/*
 * FW_BANO_Flash_v0001.cpp
 *
 * Created: 18.05.2019 23:15:44
 * Author : С_Белл
 */ 

/*
 * FW_BANO_Flash_v001.c
 *
 * Created: 18.05.2019 23:00:03
 * Author : С_Белл
 */ 

//
//
//               Reset/ADC0 PB5        VCC
//
//     XTAL1/CLK1/OC1B/ADC3 PB3        PB2(SCK/SCL/ADC1/T0/INT0
//
//     XTAL2/CLKO/OC1B/ADC2 PB4        PB1 MISO/D0/AIN1/OC0B/OC1A
//
//                          GND        PB0 MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF
//
//


/*
 * BANO_Flash_001.c
 *
 * Created: 18.05.2019 16:46:59
 * Author : Zobenko
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define DDR_SYNC  0x04  // PortB2, pin 7
#define DDR_FLASH 0x02  // PortB1, pin 6

int main() {
TCCR1 = (1<<CS02)|(1<<CS00);
TIFR = (1<<TOV1);
TIMSK = (1<<TOIE1);
// DDRB = 1;
// PORTB = 1;
DDRB = (DDR_SYNC | DDR_FLASH);
PORTB = DDR_SYNC;


sei();
while (1) {
//PORTB=0; // Don't optimize out
}
}

#define FLS_INTVL 60
#define FLS_DLY 15
#define FLS_PAUSE 500
#define SYNC_PERIOD 100

//PINB2/PORTB2 - Sync
//PORTB4 - Flash Out

ISR(TIMER1_OVF_vect) 
{
    static int cntr = 0;

    if( (cntr == FLS_INTVL * 0) || (cntr == (FLS_INTVL * 1)) || (cntr == (FLS_INTVL * 2)) )
    {
//        DDRB  |= DDR_FLASH;
        PORTB |= DDR_FLASH;

        if(cntr == FLS_INTVL * 0){PORTB |= DDR_SYNC; DDRB |= DDR_SYNC;}

        cntr ++;
    }
    else if( (cntr == FLS_INTVL * 0 + FLS_DLY) || (cntr == (FLS_INTVL * 1 + FLS_DLY)) || (cntr == (FLS_INTVL * 2 + FLS_DLY)) )
    {
        // DDRB &= ~DDR_SYNC;
        PORTB &= ~(DDR_FLASH | DDR_SYNC);
        cntr ++;
    }
    else if(  cntr == FLS_PAUSE  )
    {
		DDRB &= 0xFB; //~(DDR_SYNC);
        PORTB &= ~(DDR_SYNC);
        cntr ++;
    }
    else if(  (cntr > FLS_PAUSE)  &&  (cntr < (FLS_PAUSE + SYNC_PERIOD))  )
    {

        if((PINB & DDR_SYNC)== DDR_SYNC)
        {
            DDRB |= (DDR_SYNC);
            PORTB |= (DDR_FLASH | DDR_SYNC);
            cntr = 1;
    	}
        else
        {
            cntr ++;
        }
    }
    else if(cntr >= (FLS_PAUSE + SYNC_PERIOD))
    {
        // DDRB = 1;
        cntr = 0;
    }
    else
    {
        cntr ++;
    }

}










/*
#include <avr/io.h>
#include <avr/interrupt.h>

int cntr = 0;

ISR(TIMER1_OVF_vect) {
	
	cntr ++;
	
	if(cntr == 950
	{
		DDRB = 1;
		PORTB=1;
	}
	else if(cntr > 1000)
	{
		cntr = 0;
		DDRB = 0;
		PORTB=0;
	}

	
}

int main() {
	TCCR1 = (1<<CS02)|(1<<CS00);
	TIFR = (1<<TOV1);
	TIMSK = (1<<TOIE1);
	sei();
	while (1) {
	//	PORTB=0; // Don't optimize out
	}
}
*/