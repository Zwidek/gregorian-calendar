#include<msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"


//---------------- zmienne globalne -------------
unsigned int i=0;
unsigned int sekundy=50;
unsigned int minuty=59;
unsigned int godziny=23;
unsigned int dzien=31;
unsigned int miesiac=1;
unsigned int licznik=0;

void Clock(void);

void display_number(int x){
  if(x>=10){
    display_number(x/10);
  }
  int digit = x%10 + 48;
  SEND_CHAR(digit);
}

void InitSpecial() {
  SEND_CMD(CG_RAM_ADDR);
  int spec[2][8] = {{0, 0, 2, 4, 31, 17, 17, 17}, //ñ
                    {0, 2, 4, 31, 2, 4, 8, 31} }; //Ÿ
                    
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 8; j++) {
      SEND_CHAR(spec[i][j]);
    }
  }
}

char miesiace[12][13] = {
                    {'S', 't', 'y', 'c', 'z', 'e', 8, ' ', ' ', ' ', ' '},
                    {'L', 'u', 't', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {'M', 'a', 'r', 'z', 'e', 'c', ' ', ' ', ' ', ' ', ' '},
                    {'K', 'w', 'i', 'e', 'c', 'i', 'e', 8, ' ', ' ', ' '},
                    {'M', 'a', 'j', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                    {'C', 'z', 'e', 'r', 'w', 'i', 'e', 'c', ' ', ' ', ' '},
                    {'L', 'i', 'p', 'i', 'e', 'c', ' ', ' ', ' ', ' ', ' '},
                    {'S', 'i', 'e', 'r', 'p', 'i', 'e', 8, ' ', ' ', ' '},
                    {'W', 'r', 'z', 'e', 's', 'i', 'e', 8, ' ', ' ', ' '},
                    {'P', 'a', 9, 'd', 'z', 'i', 'e', 'r', 'n', 'i', 'k'},
                    {'L', 'i', 's', 't', 'o', 'p', 'a', 'd', ' ', ' ', ' '},
                    {'G', 'r', 'u', 'd', 'z', 'i', 'e', 8, ' ', ' ', ' '}
};                
int dni[12] = {32, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void display_month(int number) {
  int j=0;
  SEND_CMD(DD_RAM_ADDR+4);
  for(j;j<11;j++){
    SEND_CHAR(miesiace[number-1][j]);
  }
}

//----------------- main program -------------------
void main( void )
{
P2DIR |= BIT1 ;                   // STATUS LED

WDTCTL=WDTPW + WDTHOLD;           // Wy³¹czenie WDT

InitPortsLcd();                   // inicjalizacja portów LCD
InitSpecial();
InitLCD();                        // inicjalizacja LCD
clearDisplay();                   // czyszczenie wyœwietlacza      

// Basic Clock Module ustawiamy na ACLK(zegar 8 MHz ) i dzielimy czêstotliwoœæ przez 2 (4 MHz)
BCSCTL1 |= XTS;                       // ACLK = LFXT1 = HF XTAL 8MHz

do 
  {
    IFG1 &= ~OFIFG;                     // Czyszczenie flgi OSCFault
    for (i = 0xFF; i > 0; i--);         // odczekanie
    }
  while ((IFG1 & OFIFG) == OFIFG);    // dopóki OSCFault jest ci¹gle ustawiona   

BCSCTL1 |= DIVA_1;                    // ACLK=8 MHz/2=4 MHz
BCSCTL2 |= SELM0 | SELM1;             // MCLK= LFTX1 =ACLK

// Timer_A  ustawiamy na 500 kHz
// a przerwanie generujemy co 100 ms
TACTL = TASSEL_1 + MC_1 +ID_3;        // Wybieram ACLK, ACLK/8=500kHz,tryb Up
CCTL0 = CCIE;                         // w³¹czenie przerwañ od CCR0
CCR0=5000;                           // podzielnik 5000: przerwanie co 10 ms

_EINT();                              // w³¹czenie przerwañ
SEND_CHAR('3');
SEND_CHAR('1');
display_month(1);
SEND_CMD(DD_RAM_ADDR2);
SEND_CHAR('2');
SEND_CHAR('3');
SEND_CHAR(':');
SEND_CHAR('5');
SEND_CHAR('9');
SEND_CHAR(':');
SEND_CHAR('0');
SEND_CHAR('0');
SEND_CHAR(':');
SEND_CHAR('0');
SEND_CHAR('0');
for (;;)                              
{
  _BIS_SR(LPM3_bits);                  // przejscie do trybu LPM3
  Clock();
}
}

void Clock(void)
{
  SEND_CMD(DD_RAM_ADDR2+9);
  display_number(licznik%100);

  if (licznik %100 == 0)                    // gdy mine³a sekunda (10 * 100 milisekund)
  {
    
    licznik=0;
    SEND_CMD(DD_RAM_ADDR2+6);
    P2OUT ^= BIT1;                           //zapal diodê
    ++sekundy;                           // licz sekundy
    sekundy = sekundy%60;
    if(sekundy<10){
      SEND_CHAR('0');
      display_number(sekundy);              //wyswietl liczbe sekund
    }
    else{
      display_number(sekundy);              //wyswietl liczbe sekund
    }
    if(sekundy==0){
      minuty++;
      minuty = minuty%60;
      SEND_CMD(DD_RAM_ADDR2+3);
      if(minuty<10){
        SEND_CHAR('0');
        display_number(minuty);              //wyswietl liczbe minut
      }
      else {
        display_number(minuty);              //wyswietl liczbe minut
      }
      if(minuty==0){
        godziny++;
        godziny = godziny%24;
        SEND_CMD(DD_RAM_ADDR2);
        if(godziny<10){
          SEND_CHAR('0');
          display_number(godziny);              //wyswietl liczbe godzin
        }
        else {
          display_number(godziny);              //wyswietl liczbe godzin
        }
        if(godziny==0){
          dzien++;
          dzien = dzien % dni[miesiac-1];
          if(dzien==0){
            miesiac++;
            display_month(miesiac);
            dzien++;
          }
          SEND_CMD(DD_RAM_ADDR);
          if(dzien < 10){
            SEND_CHAR('0');
            display_number(dzien);
          }
          else {
            display_number(dzien);
          }
        }
      }
    }
  }
}

// procedura obs³ugi przerwania od TimerA

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  ++licznik;
  _BIC_SR_IRQ(LPM3_bits);             // wyjœcie z trybu LPM3
}
