#include <Stepper.h>
#include <LiquidCrystal.h>
#include <time.h>
#include <Arduino_FreeRTOS.h>

#define portCHAR char

const uint16_t stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor

uint8_t lcdD4Pin = 5;
uint8_t lcdD5Pin = 4;
uint8_t lcdD6Pin = 3;
uint8_t lcdD7Pin = 2;
uint8_t lcdEPin = 12;
uint8_t lcdRSPin = 13;
volatile uint8_t switchDirection = 0;
uint8_t flag = 1;
enum directions{Clockwise, CounterClockwise};

void TaskMotor( void *pvParameters );
//void TaskTimer( void *pvParameters );

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,10,11,9);
LiquidCrystal lcd(lcdRSPin, lcdEPin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);

void setup() {
  
  cli();//stop interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("hello, world");

  sei();//allow interrupts

  
  //xTaskCreate(TaskTimer, (const portCHAR *)"T", 128, NULL, 1, NULL); 
  xTaskCreate(TaskMotor, (const portCHAR *)"M", 128, NULL, 1, NULL);
  
}

void loop() {
 
}

/*
void TaskTimer(void *pvParameters) {
  (void) pvParameters;
  Serial.begin(9600);
 
  lcd.begin(16,2);
  lcd.print("hello, world");

  for(;;) {
    if(flag) {
      flag = 0;
      lcd.setCursor(0,0);
      Serial.print("Half a second...");
      if(switchDirection){
        direction = "CounterClockwise";
        Serial.print("Counter");
      }
      else {
        direction = "ClockWise";
        Serial.print("Clockwise");
      }
      lcd.print(direction);
      vTaskDelay(1);
    }
  }
}
*/
void TaskMotor(void *pvParameters) {
  (void) pvParameters;
 
  myStepper.setSpeed(9);
  for(;;) {
    switchDirection = 1;
    Serial.print(switchDirection);
    myStepper.step(stepsPerRevolution);
    
    
    switchDirection = 0;
    Serial.print(switchDirection);
    myStepper.step(-stepsPerRevolution);
    
    vTaskDelay(1);
  }
}
ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
    if(flag) {
      lcd.setCursor(0,0);
      lcd.clear();
      if(switchDirection){
        lcd.print("CounterClockwise");
        Serial.print("Counter\n");
      }
      else {
        lcd.print("Clockwise");
        Serial.print("Clockwise\n");
      }
      vTaskDelay(1);
    }
}

