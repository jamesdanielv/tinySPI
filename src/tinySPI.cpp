// Arduino tinySPI Library Copyright (C) 2018 by Jack Christensen and
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino hardware SPI master library for
// ATtiny24/44/84, ATtiny25/45/85, ATtiny261/461/861, ATtiny2313/4313.
//
// https://github.com/JChristensen/tinySPI
// Jack Christensen 24Oct2013
//includeded minor enhancements James Villeneuve
#define speedOverSize true //if true faster performance however it uses 24 more bytes
#define IdoNotUseInterrupts true //if you do no use interrups specifically USI, set this to true to save 10 bytes!
#include <tinySPI.h>

void tinySPI::begin()
{
    USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
    USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
    SPI_DDR_PORT |= _BV(USCK_DD_PIN);   // set the USCK pin as output
    SPI_DDR_PORT |= _BV(DO_DD_PIN);     // set the DO pin as output
    SPI_DDR_PORT &= ~_BV(DI_DD_PIN);    // set the DI pin as input
}

void tinySPI::setDataMode(uint8_t spiDataMode)
{
    if (spiDataMode == SPI_MODE1)
        USICR |= _BV(USICS0);
    else
        USICR &= ~_BV(USICS0);
}

uint8_t tinySPI::transfer(uint8_t spiData)
{
    USIDR = spiData;
    USISR = _BV(USIOIF);                // clear counter and counter overflow interrupt flag
    #if IdoNotUseInterrupts !=true
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)   // ensure a consistent clock period
    {
    #endif
#if speedOverSize == true //if this is true we use 24 more bytes but get faster performance
USICR |= _BV(USITC);USICR |= _BV(USITC);USICR |= _BV(USITC); ;USICR |= _BV(USITC);
USICR |= _BV(USITC);USICR |= _BV(USITC);USICR |= _BV(USITC); ;USICR |= _BV(USITC);
USICR |= _BV(USITC);USICR |= _BV(USITC);USICR |= _BV(USITC); ;USICR |= _BV(USITC);
USICR |= _BV(USITC);USICR |= _BV(USITC);USICR |= _BV(USITC); ;USICR |= _BV(USITC);
#else
while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
#endif //speedOverSize 
    #if IdoNotUseInterrupts !=true 
    }
    #endif
    return USIDR;
}

void tinySPI::end()
{
    USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
}

tinySPI SPI;
