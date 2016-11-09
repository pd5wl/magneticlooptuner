/*-----( Import needed libraries )-----*/
#include "Stepper.h"
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
/*#include <SoftwareSerial.h>
#include <FT857D.h>*/

/*-----( Declare Constants )-----*/
#define STEPS  32   // Number of steps for one revolution of Internal shaft
                    // 2048 steps for one revolution of External shaft
#define BACKLIGHT_PIN 13

/*-----( Declare objects )-----*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
Stepper small_stepper(STEPS, 8, 10, 9, 11); // Setup of proper sequencing for Motor Driver Pins
                                            // In1, In2, In3, In4 in the sequence 1-3-2-4
                                            
/*-----( Declare Variables )-----*/ 
volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation
 
const int PinCLK=2;   // Generating interrupts using CLK signal
const int PinDT=3;    // Reading DT signal
const int PinSW=4;    // Reading Push Button switch
 
int RotaryPosition=0;    // To store Stepper Motor Position
 
int PrevPosition;     // Previous Rotary position Value to check accuracy
int StepsToTake;      // How much to move Stepper

// Interrupt routine runs if CLK goes from HIGH to LOW
void isr ()  {
  delay(4);  // delay for Debouncing
  if (digitalRead(PinCLK))
    rotationdirection= digitalRead(PinDT);
  else
    rotationdirection= !digitalRead(PinDT);
  TurnDetected = true;
}
 
void setup ()  {
// LCD
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("MagLoopTunner");
  lcd.setCursor(0,1);
  lcd.print("PD5WL/Ver. 0.1b");
   // Switch on the backlight
  pinMode ( BACKLIGHT_PIN, OUTPUT );
  digitalWrite ( BACKLIGHT_PIN, HIGH ); 
  delay(3000);
  lcd.clear();
  lcd.print("Rotorposition");
// Rotery  
  pinMode(PinCLK,INPUT);
  pinMode(PinDT,INPUT);  
  pinMode(PinSW,INPUT);
  digitalWrite(PinSW, HIGH); // Pull-Up resistor for switch
  attachInterrupt (0,isr,FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO
}
 
void loop ()  {

  small_stepper.setSpeed(600); //Max seems to be 700
  if (!(digitalRead(PinSW))) {   // check if button is pressed
    if (RotaryPosition == 0) {  // check if button was already pressed
    } else {
        small_stepper.step(-(RotaryPosition*50));
        RotaryPosition=0; // Reset position to ZERO
      }
    }
 
  // Runs if rotation was detected
  if (TurnDetected)  {
    PrevPosition = RotaryPosition; // Save previous position in variable
    if (rotationdirection) {
      RotaryPosition=RotaryPosition-1;} // decrase Position by 1
    else {
      RotaryPosition=RotaryPosition+1;} // increase Position by 1
 
    TurnDetected = false;  // do NOT repeat IF loop until new rotation detected
 
    // Which direction to move Stepper motor
    if ((PrevPosition + 1) == RotaryPosition) { // Move motor CW
      StepsToTake=50; 
      small_stepper.step(StepsToTake);
    }
 
    if ((RotaryPosition + 1) == PrevPosition) { // Move motor CCW
      StepsToTake=-50;
      small_stepper.step(StepsToTake);
    }
  }
  lcd.setCursor(3,1);
  lcd.print(RotaryPosition);  
}



