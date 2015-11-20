#define SIN_LEN_QUARTER 8
#define SIN_LEN         (SIN_LEN_QUARTER*4)
#define CLOCK_FREQ      16000000
#define SINE_DIVISOR    8
#define SINE_INT_OVF    17

char sineArray[SIN_LEN_QUARTER + 1];

int actPin0 = 9;
int actPin1 = 10;
volatile int timer = 0;
volatile int overflow = 100;
volatile int go = 0;
//volatile char test;

void buildSin( void )
{
  int i;
  /* Create 1/4 sin wave + 1 point. */
  for ( i = 0; i <= SIN_LEN_QUARTER; i++ )
  {
    sineArray[i] = (char)( 127.5 * sin( 2.0 * PI * i / SIN_LEN ) );
  }
}

char sine( unsigned int index )
{
  index &= SIN_LEN - 1;
  
  if ( index <= SIN_LEN_QUARTER )
    return sineArray[index];
  else if ( index <= 2 * SIN_LEN_QUARTER )
    return sineArray[ 2 * SIN_LEN_QUARTER - index ];
  else if ( index <= 3 * SIN_LEN_QUARTER )
    return -sineArray[ index - 2 * SIN_LEN_QUARTER ];
  else
    return -sineArray[ SIN_LEN - index ];
}

void testSine( void )
{
  int i; 
  
  Serial.print( "Sine:\r\n" );
  
  for ( i = 0; i <= SIN_LEN*2; i++ )
  {
    Serial.print( (int)sine(i) );
    Serial.print( "\r\n" );
  }
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

/*
ISR(TIMER1_OVF_vect)
{
}
*/

ISR(TIMER2_COMPA_vect)
{
//  test = 1;
  if ( ++timer >= overflow )
  {
    timer = 0;
    go = 1;
  }
}

void setFreq( int hz )
{
  if ( hz == 0 )
  {
    TIMSK2 &= ~_BV(OCIE2A);      // Disable Compare 2A Interrupt
    analogWrite( actPin0, 0 );
    analogWrite( actPin1, 0 );
    go = 0;
  }
  else
  {
    hz = constrain( hz, 1, 100 );
    overflow = ( (long long)CLOCK_FREQ * 2 / SIN_LEN / SINE_DIVISOR / 
                 SINE_INT_OVF / hz + 1 ) / 2;
    TIMSK2 |= _BV(OCIE2A);      // Enable Compare 2A Interrupt
  }
}

float getFreq( void )
{
  return (float)CLOCK_FREQ / SIN_LEN / SINE_DIVISOR /
         SINE_INT_OVF / overflow;
}

void setup()
{
  buildSin();
//  testSine();
  
  Serial.begin(9600);

  setPwmFrequency(actPin0, 1);              // Also applies to pin 9.
  setPwmFrequency(3, SINE_DIVISOR);         // Set timer0 frequency for sine.
  TCCR1A = ( TCCR1A & 0x0F ) | 0b11100000;  // Inverting pin 10.
  pinMode( actPin0, OUTPUT );
  pinMode( actPin1, OUTPUT );
  
//  TIMSK1 |= _BV(TOV1);
//  TCCR2A = _BV(COM2A0) | _BV(WGM21);
  
  cli();
  
  /* CTC mode on timer 2 */
  TCCR2A = 0;                 // Clear timer mode
  TCCR2A |=  0b00000010;      // WGM20, WGM21 on bit 0, 1
  TCCR2B &= ~0b00001000;      // WGM22 on bit 3
  OCR2A = SINE_INT_OVF - 1;   // Sine Overflow
  TIMSK2 |= _BV(OCIE2A);      // Enable Compare 2A
  
  sei();
  
//  SREG |= 0x80;
} 

void loop()
{
  long rate = 100;
//  float angle = 0;
  int angle = 0;
  char value;
  
//  while ( 1 );
  
  setFreq( 1 );
  
  while ( 1 )
  {
    while ( Serial.available() > 0 )
    {
      int hz = Serial.parseInt(); 
      
      if ( Serial.read() == '\n' )
      {
        setFreq( hz );
        Serial.print( hz );
        Serial.print( "Hz=" );
        if ( hz == 0 )
        {
          Serial.print( "Off" );
        }
        else
        {
          Serial.print( overflow );
          Serial.print( "=" );
          Serial.print( getFreq() );
          Serial.print( "Hz" );
        }
        Serial.print( "\r\n" );
      }
    }
    
    if ( go == 1 )
    {
      go = 0;
//      test = 0;
      
//      value = (char)( ( sin( angle ) + 1 ) / 2 * 254 + 1 );
//      angle += 0.02;
      
      value = sine( angle++ ) + 128;
      angle &= SIN_LEN - 1;
      
      analogWrite( actPin0, value );
      analogWrite( actPin1, value );
      
//      delay( 10 );
      
      if ( go == 1 )
         Serial.print( "!\r\n" );
      
//      Serial.print( (int)(TCCR2A&0b11 | (TCCR2B>>1)&0b100) );
//      Serial.print( (int)OCR2A );
//      Serial.print( "\n" );
    }
    
//    delayMicroseconds( rate * 100000 );
  }
} 

