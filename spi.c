#include <avr/io.h>
//#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include "pinDefines.h"
#include "spi.h"

void initSPImaster(void)
{
	SPI_SS_DDR |= (1 << SPI_SS);				  /* set SS output */
	SPI_SS_PORT |= (1 << SPI_SS);				  /* start off not selected (high) */

	SPI_MOSI_DDR |= (1 << SPI_MOSI);			  /* output on MOSI */
	SPI_MISO_DDR |= (0 << SPI_MISO);			  /* input on MISO */
	SPI_MISO_PORT |= (1 << SPI_MISO);			  /* pullup on MISO */
	SPI_SCK_DDR |= (1 << SPI_SCK);				  /* output on SCK */

/* Don't have to set phase, polarity b/c
 * default works with 25LCxxx chips */
	// 16K baud
	SPCR |= (1 << SPR0);
	SPCR |= (1 << SPR1);

//	SPCR &= (0 << CPOL);
//	SPCR &= (0 << CPHA);

	// 125K baud
//	SPCR |= (1 << SPR0);

	// 33K baud
//	SPCR |= (1 << SPR1);

//  otherwise if SPR0 & SPR1 not set: 500K baud
	
	SPCR |= (1 << MSTR);						  /* clockmaster */
	SPCR |= (1 << SPE);							  /* enable */
}

void initSPIslave(void)
{
	SPI_SS_DDR |= (0 << SPI_SS);				  /* set SS input */
	SPI_SS_PORT |= (1 << SPI_SS);				  /* start off not selected (high) */

	SPI_MISO_DDR |= (1 << SPI_MISO);			  /* output on MOSI */
	SPI_MOSI_DDR |= (0 << SPI_MOSI);			  /* input on MOSI */
//	SPI_MOSI_PORT |= (1 << SPI_MOSI);			  /* pullup on MISO */
	SPI_SCK_DDR |= (0 << SPI_SCK);				  /* input on SCK */

//	SPCR &= (0 << CPOL);
//	SPCR &= (0 << CPHA);
	SPCR |= (1 << SPE);							  /* enable */
}

void SPI_write(uint8_t byte)
{
	SPI_SS_PORT &= 0xFB;	// B2 is SS
	SPDR = byte;								  /* SPI starts sending immediately */

//	loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
	while(!(SPSR & (1<<SPIF)));
	SPI_SS_PORT |= 0x04;
/* SPDR now contains the received byte */
}

// from avr/include/avr/sfr_defs.h
//#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
// instead of polling here or using interrupt, modify the above macro
uint8_t SPI_read(void)
{
	loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
	return SPDR;
/* SPDR now contains the received byte */
}

