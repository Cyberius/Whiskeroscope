#define SIN_LEN_QUARTER 8
#define SIN_LEN         (SIN_LEN_QUARTER*4)
#define CLOCK_FREQ      16000000
#define SINE_DIVISOR    8
#define SINE_INT_OVF    17

#define MAX_FREQ        100
#define MAX_AMPL        100
#define MAX_RATE        100

char sineArray[SIN_LEN_QUARTER + 1];

int actPin0 = 9;
int actPin1 = 10;
volatile int timer = 0;
volatile int overflow = 100;
volatile int go = 0;
char freq;
char ampl = MAX_AMPL;
char fsweep_freq_start;
char fsweep_freq_end;
int fsweep_rate;
volatile int fsweep_count;
char fsweeping = 0;

void buildSin( char ampl )
{
  int i;
  /* Create 1/4 sin wave + 1 point. */
  for ( i = 0; i <= SIN_LEN_QUARTER; i++ )
  {
    sineArray[i] = (char)( 127.5 * ampl * sin( 2.0 * PI * i / SIN_LEN ) / MAX_AMPL );
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
  if ( ++timer >= overflow )
  {
    timer = 0;
    go = 1;
  }
  
  fsweep_count--;
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
    hz = constrain( hz, 1, MAX_FREQ );
    overflow = ( (long long)CLOCK_FREQ * 2 / SIN_LEN / SINE_DIVISOR / 
                 SINE_INT_OVF / hz + 1 ) / 2;
    TIMSK2 |= _BV(OCIE2A);      // Enable Compare 2A Interrupt
  }
  
  freq = hz;
}

float getFreq( void )
{
  return (float)CLOCK_FREQ / SIN_LEN / SINE_DIVISOR /
         SINE_INT_OVF / overflow;
}

void setup()
{
  buildSin( MAX_AMPL );
//  testSine();
  
  Serial.begin(9600);

  setPwmFrequency(actPin0, 1);              // Also applies to pin 9.
  setPwmFrequency(3, SINE_DIVISOR);         // Set timer0 frequency for sine.
  TCCR1A = ( TCCR1A & 0x0F ) | 0b11100000;  // Inverting pin 10.
  pinMode( actPin0, OUTPUT );
  pinMode( actPin1, OUTPUT );
  
  cli();
  
  /* CTC mode on timer 2 */
  TCCR2A = 0;                 // Clear timer mode
  TCCR2A |=  0b00000010;      // WGM20, WGM21 on bit 0, 1
  TCCR2B &= ~0b00001000;      // WGM22 on bit 3
  OCR2A = SINE_INT_OVF - 1;   // Sine Overflow
  TIMSK2 |= _BV(OCIE2A);      // Enable Compare 2A
  
  sei();
} 

void loop()
{
  typedef enum
  {
    MODE_NONE = 0,
    MODE_INVALID,
    MODE_FREQ,
    MODE_AMPL,
    MODE_FSWEEP,
  } E_COMMS_MODE;
  
  long rate = 100;
  int angle = 0;
  char value;
  E_COMMS_MODE comms_mode = MODE_NONE;
  
  setFreq( 1 );
  
  while ( 1 )
  {
    while ( Serial.available() > 0 )
    {
      int val_int;
      char mode_char;
      
      switch ( comms_mode )
      {
        case MODE_NONE:
          mode_char = Serial.read(); 
          
          switch ( mode_char )
          {
            case 'F':
            case 'f':
              comms_mode = MODE_FREQ;
              break;
              
            case 'A':
            case 'a':
              comms_mode = MODE_AMPL;
              break;
              
            case 'S':
            case 's':
              comms_mode = MODE_FSWEEP;
              break;
              
            default:
              comms_mode = MODE_INVALID;
              break;
          }
          break;
          
        case MODE_FSWEEP:
          fsweep_freq_end = Serial.parseInt();
        case MODE_FREQ:
        case MODE_AMPL:
          val_int = Serial.parseInt();
          break;
        default:
          break;
      }
      
      if ( Serial.read() == '\n' )
      {
        switch ( comms_mode )
        {
          case MODE_FREQ:
            setFreq( val_int );
            Serial.print( val_int );
            Serial.print( "Hz=" );
            if ( val_int == 0 )
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
            break;
            
          case MODE_AMPL:
            ampl = constrain( val_int, 0, MAX_AMPL );
            Serial.print( "Ampl=" );
            Serial.print( (int)ampl );
            buildSin( ampl );
            break;
            
          case MODE_FSWEEP:
            fsweep_freq_start = freq;
            fsweep_freq_end = constrain( fsweep_freq_end, 1, MAX_FREQ );
            fsweep_rate = constrain( val_int, 1, MAX_RATE );
            Serial.print( "Sweep " );
            Serial.print( (int)fsweep_freq_start );
            Serial.print( "Hz to " );
            Serial.print( (int)fsweep_freq_end );
            Serial.print( "Hz at " );
            Serial.print( (int)fsweep_rate );
            Serial.print( "t/Hz" );
            
            fsweep_rate *= 320;
            fsweep_count = 0;
            
            /* Only start sweep if not off */
            fsweeping = ( fsweep_freq_start != 0 );
            break;
            
          default:
            Serial.print( "Invalid" );
            break;
        }
        
        Serial.print( "\r\n" );
        comms_mode = MODE_NONE;
      }
    }
    
    if ( go == 1 )
    {
      go = 0;
      value = sine( angle++ ) + 128;
      angle &= SIN_LEN - 1;
      
      analogWrite( actPin0, value );
      analogWrite( actPin1, value );
      
      if ( fsweeping )
      {
        if ( fsweep_count <= 0 )
        {
          /* Finished 1 rate period */
          
          /* Adjust for one more rate period */
          fsweep_count += fsweep_rate;
          
          /* Calculate and set new frequency */
          if ( freq != fsweep_freq_end )
          {
            /* We are still ramping */
            if ( freq < fsweep_freq_end )
              freq++;
            else
              freq--;
            
            setFreq( freq );
            /*
            Serial.print( (int)freq );
            Serial.print( "\r\n" );
            /**/
          }
          else
          {
            fsweeping = 0;
            Serial.print( "Sweep Done\r\n" );
          }
        }
      }
      
      if ( go == 1 )
         Serial.print( "!\r\n" );
    }
  }
} 

