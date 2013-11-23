// original code by Marc Griffith rchacker.com is licensed under a Creative Commons Attribution 3.0 Unported License.
// http://creativecommons.org/licenses/by/3.0/deed.en_US
// additions by marcino239  github.com/marcino239
// 

#include <KK2LCD.h>

//AIL, THR etc
//can also be digital outputs

const byte IN1 = 0;  //PD3 (PCINT27/TXD1/INT1)  not tested, but use Serial1 
const byte IN2 = 1;  //PD2 (PCINT26/RXD1/INT0)  interrupts good for CCPM decoding.
const byte IN3 = 2;  //PD0 (PCINT24/RXD0/T3)  tx0 is on the lcd not sure if using this would conflict with the lcd  
const byte IN4 = 3;  //PB2 (PCINT10/INT2/AIN0)
const byte IN5 = 4;  //PB0 (PCINT8/XCK0/T0)  //timer/counter0 source

//motor outputs can also be digital inputs. these also have PCINT16 to 23 Arduino interrupts not tested.
const byte OUT1 = 5;  //PC6 (TOSC1/PCINT22)   //32.768kHz crystal or custom clock source for counter (rpm sensor)
const byte OUT2 = 6;  //PC4 (TDO/PCINT20)   //JTAG 
const byte OUT3 = 7;  //PC2 (TCK/PCINT18)   //JTAG
const byte OUT4 = 8;  //PC3 (TMS/PCINT19)  //JTAG
const byte OUT5 = 9;  //PC1 (SDA/PCINT17)  //I2C      i2c not tested
const byte OUT6 = 10; //PC0 (SCL/PCINT16)  //I2C
const byte OUT7 = 11; //PC5 (TDI/PCINT21)   //JTAG
const byte OUT8 = 12; //PC7 (TOSC2/PCINT23)   //32.768kHz crystal

const byte RED_LED = 13;  //PB3 (PCINT11/OC0A/AIN1)  //same as arduino!

//important enable the internal pullups when using these as inputs
const byte BUT1 = 14;  //PB7 (PCINT15/OC3B/SCK)    PWM     pwm not tested
const byte BUT2 = 15;  //PB6 (PCINT14/OC3A/MISO)   PWM
const byte BUT3 = 16;  //PB5 (PCINT13/ICP3/MOSI)
const byte BUT4 = 17;  //PB4 (PCINT12/OC0B/SS)

const byte _BUZZER = 18;  //PB1 (PCINT9/CLKO/T1)   CLOCK output can adjust with system prescaler. (make tones) not tested

//uncomment if you want to write your own LCD library
/*
const byte LCD_CS1 = 19;
const byte LCD_RES = 20;
const byte LCD_A0 = 21;
const byte LCD_SCL = 22;
const byte LCD_SI = 23;
*/

//analog reads must be done using thier channels, specifying digital pin numbers will not work in this case
const byte BATT = 3;

const byte GYR_R = 1;
const byte GYR_Y = 2;
const byte GYR_P = 4;

const byte ACC_X = 5;
const byte ACC_Y = 6;
const byte ACC_Z = 7;


// pin definition for l298
#define ENA 5  // OUT1
#define IN1 6  // OUT2
#define IN2 7  // OUT3

// debounce constants
const int switch_release_debounce_us = 100;
const int switch_press_debounce_us = 500;

// every time I write debounce code it comes out different. Complicated but I like the feel of it.
byte button4Pressed()
{
  if(!digitalRead(BUT4))
  {
    delayMicroseconds(switch_press_debounce_us);
    if(!digitalRead(BUT4))
    {
      while(!digitalRead(BUT4))
      {
        st7565SetBrightness(12);
        st7565ClearBuffer();
        st7565SetFont( Font12x16 );
        st7565DrawString_P( 42, 26 ,  PSTR("Next") );
        st7565Refresh();
        digitalWrite(RED_LED,HIGH);
        //we could put a beep in here too.
      }
      delayMicroseconds(switch_release_debounce_us);
      digitalWrite(RED_LED,LOW);    
      return 1;
    }
  }
  return 0;
}


void setup() {
  
  // put your setup code here, to run once:
  Serial1.begin( 9600 );
  Serial1.println( PSTR( "kkbot test ready" ) );
 

  // init pins
  pinMode(RED_LED, OUTPUT); 

  pinMode(GYR_R, INPUT);
  pinMode(GYR_Y, INPUT);
  pinMode(GYR_P, INPUT);

  pinMode(ACC_X, INPUT);
  pinMode(ACC_Y, INPUT);
  pinMode(ACC_Z, INPUT);

  pinMode(BUT1,INPUT);
  digitalWrite(BUT1, HIGH);   //enable internal pullup.

  pinMode(BUT2,INPUT);
  digitalWrite(BUT2, HIGH);

  pinMode(BUT3,INPUT);
  digitalWrite(BUT3, HIGH);

  pinMode(BUT4,INPUT);
  digitalWrite(BUT4, HIGH);

  analogReference(EXTERNAL); //important!!

  st7565Init( Font5x7 );
  st7565SetBrightness(12);
  st7565DrawString_P( 64, 40, PSTR("Arduino on") );
  st7565DrawString_P( 64, 32, PSTR(" the KK2.") );  
  st7565DrawString_P( 64, 24, PSTR("Test suite") );  
  st7565DrawString_P( 64, 16, PSTR("   v1.0  ") );
  st7565DrawString_P( 64, 8, PSTR( "  Marc G") );
  st7565DrawString_P( 50, 1, PSTR("Press any key") );
  st7565WriteLogo(); //see library to modify

  // set up pins to drive l298
  pinMode( ENA, OUTPUT );  
  pinMode( IN1, OUTPUT );  
  pinMode( IN2, OUTPUT );  
  
  delay(1000);
  button4Pressed();  

  st7565ClearBuffer();
  st7565SetBrightness(12);
  st7565SetFont( Font5x7 );
}


void loop()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
 
  st7565DrawString_P(10*6,4,PSTR("Accelerate "));
  for(byte i = 0; i < 256; i++) //Accelerate
  {
    analogWrite(ENA, i);
    delay(100);
  }
 
  st7565DrawString_P(10*6,4,PSTR("Holding top spered "));
  for(byte j = 5; j > 0; j--) //Hold top speed
  {
    delay(1000);
  }

  digitalWrite (IN1, LOW); //Switch direction
  digitalWrite(IN2, HIGH);
 
  st7565DrawString_P(10*6,4,PSTR("Changing speed"));
  for(byte h = 0; h < 256; h++) //Accelerate
  {
    analogWrite(ENA, h);
    delay(100);
  }
 
  st7565DrawString_P(10*6,4,PSTR("Rvrs Holding speed"));
  for(byte u = 5; u > 0; u--) //Hold top speed
  {
    delay(1000);
  }
}


