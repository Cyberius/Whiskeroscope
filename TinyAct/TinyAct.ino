#define MAX_COUNT 10

int actPin0 = 9;
int actPin1 = 10;
volatile int timer = 0;
volatile int overflow = 100;
volatile int go = 0;
  

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

ISR(TIMER1_OVF_vect)
{
}

ISR(TIMER2_COMPA_vect)
{
//  TCNT2 = 0x0;
  
  if ( ++timer >= overflow )
  {
    go = 1;
    timer = 0;
//    Serial.print( "a\n" );
  }
}

void setup()
{
  Serial.begin(9600);

  setPwmFrequency(actPin0, 1);  // Also applies to pin 9.
  setPwmFrequency(3, 1);  // We're just setting timer0 frequency.
  TCCR1A = ( TCCR1A & 0x0F ) | 0b11100000;  // Inverting pin 10.
  pinMode( actPin0, OUTPUT );
  pinMode( actPin1, OUTPUT );
  
//  TIMSK1 |= _BV(TOV1);
  TIMSK2 |= _BV(OCIE2A);
  OCR2A = 255;
//  TCCR2A = _BV(COM2A0) | _BV(WGM21);
  
//  SREG |= 0x80;
} 

void setFreq( int hz )
{
  hz = constrain( hz, 1, 100 );
  overflow = 100 / hz;
//  OCR2A = 255 - hz;
}

void loop()
{
  long rate = 100;
  float angle = 0;
  char value;
  
  setFreq( 1 );
    
  while ( 1 )
  {
    while ( Serial.available() > 0 )
    {
      int hz = Serial.parseInt(); 
      
      if ( Serial.read() == '\n' )
      {
        setFreq( hz );
        Serial.print( overflow );
        Serial.print( "\r\n" );
      }
    }
    
    if ( go == 1 )
    {
      go = 0;
      
      value = (char)( ( sin( angle ) + 1 ) / 2 * 254 + 1 );
      angle += 0.02;
      
      analogWrite( actPin0, value );
      analogWrite( actPin1, value );
    }
    
//    delayMicroseconds( rate * 100000 );
  }
} 

