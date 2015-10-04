#include "pitches.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



// CONFIG
// ------
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
const int BUTTON_ENTER = 6;
const int BUTTON_MODE = 7;
const int beepDuration = 512;
const int chirpDuration = 12;
const int debounceDelay = 50;


// STATE VARS
// ----------
int state = 0; // 0 is menu, 1 is timer running, 2 is timer stopped

unsigned long timeElapsed;
unsigned long timeStart;
boolean lastEnterReading = false;
boolean lastModeReading = false;
boolean enterValidPress = false;
boolean modeValidPress = false;
unsigned long timeLastEnterChange = 0;
unsigned long timeLastModeChange = 0;



int centiseconds;
int seconds;
int minutes;

unsigned long timeLastChirp = 0;
unsigned long timeLastBeep = 0;


// INIT
// ----

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// the top ofAdafruit_SDD1306.h needs to have #define SSD1306_128_32 uncommented
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


void setup() {
  //Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(BUTTON_ENTER, INPUT);
  pinMode(BUTTON_MODE, INPUT);
  //tone(8, NOTE_G7, beepDuration);
  //tone(8, 1000, 512);
  //delay(2000);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  
  // set some test text
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("IPSC");
  display.display();
}

void loop() {
  controlButtons();
  

  
//  display.setTextSize(1);
//  display.setCursor(120, 20);
//  display.clearDisplay();
//  display.print(state);
//  display.display();
}

void controlState() {
  

  if (state == 0) {
    runMenu();
  }
  
  else if (state == 1) {
    runTimer();
  }
  
  else if (state == 2) {
    runEnd();
  }
    
}





void runMenu() {
  
  // show menu only if chirp or beep not playing
  if ((millis() - timeLastChirp) > chirpDuration) {
    
    // display menu text once chirp has ended
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Stopwatch");
    display.display();
    
  }
}


void runEnd() {
  
//  
//  if (buttonState == 1) {
//    //chirp();
//    state = 1;
//  }
//  
//  if (buttonState == 1) {
//    // resetting time
//    //chirp();
//    state = 0;
//  }
}



void controlButtons() {
  boolean enterReading = digitalRead(BUTTON_ENTER);
  boolean modeReading = digitalRead(BUTTON_MODE);
  
  
  
  // ENTER BUTTON PROCESSING
  // -----------------------
  
  if (enterReading != lastEnterReading) {
    // there has been a change
    timeLastEnterChange = millis();
  }
   
//  Serial.print("enterReading: ");
//  Serial.print(enterReading);
//  Serial.print(" lastEnterReading: ");
//  Serial.print(lastEnterReading);
//  Serial.print(" millis: ");
//  Serial.print(millis());
//  Serial.print(" timeLastEnterChange: ");
//  Serial.print(timeLastEnterChange);
//  Serial.print(" debounce delay: ");
//  Serial.print(debounceDelay);
//  Serial.print(" total: ");
//  Serial.println(millis() - timeLastEnterChange);
  if ((millis() - timeLastEnterChange) > debounceDelay) {
    
    if (enterReading == true) {
      // enter button is pressed
      
      // dont allow rapid fire
      // button must be released before next valid press event
      if (enterValidPress) {
        enterValidPress = false;
  
        
        // if on menu
        //   start timer
        //   beep
        //   advance to timer display
        if (state == 0) {
          timeStart = millis();
          beep();
          state = 1;
        }
        
        // if on timer
        //   pause timer
        //   chirp
        else if (state == 1) {
          state = 2;
          chirp();
        }
        
        // if paused on timer
        //   resume timer
        //   chirp
        else if (state == 2) {
          state = 1;
          chirp();
        }
      }
    }
    
    // button not pressed
    else {
      enterValidPress = true;
    }
  }
  
  
  
  // MODE BUTTON PROCESSING
  // ----------------------
  
  
  if (modeReading != lastModeReading) {
    // there has been a change
    timeLastModeChange = millis();
  }
   
  if ((millis() - timeLastModeChange) > debounceDelay) { 
    if (modeReading == true) {
      // mode button is pressed
      
      // dont allow rapid fire
      // button must be released before another press event registers
      if (modeValidPress) {
        modeValidPress = false;
        
        // if on menu
        //   chirp
        if (state == 0) {
          chirp();
          // no effect
          //state = 8; // could be some other cool mode
        }
        
        // if on timer
        //   no effect
        else if (state == 1) {
          // no effect
        }
        
        // if paused on timer
        //   go back to menu (reset timer)
        //   chirp
        else if (state == 2) {
          state = 0;
          chirp();
        }
      }
      
    }
    
      // button not pressed
      //   this clears the toggle disabling button press events
      //   allow button to be pressed again
    else {
      modeValidPress = true;
    }
  }
  
  
  
  
  controlState();

  
  lastEnterReading = enterReading;
  lastModeReading = modeReading;
}





void beep() {
  tone(8, NOTE_G7, beepDuration); 
  timeLastBeep = millis();
}


void chirp() {
  //if ((millis() - timeLastChirp) > 3000) {
    tone(8, NOTE_C7, chirpDuration);
    timeLastChirp = millis();
  //}
}







void runTimer() {

  
  // increment the timer
  timeElapsed = (millis() - timeStart);
  
  
  // update the display (only if beep has elapsed)
  if ((millis() - timeLastBeep) > beepDuration) {
    // only update screen if chirp isnt chirping
      
    centiseconds = (timeElapsed / 10) % 100;     // displays 0-99
    seconds = (timeElapsed / 1000) % 60;        // displays 0-59
    minutes = (timeElapsed / 1000 / 60) % 100;   // displays 0-99
  
    

    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0, 0);
    
    // minutes
    if (minutes < 10) display.print(0);
    display.print(minutes);
    display.print(":");
    
    // seconds
    if (seconds < 10) display.print(0);
    display.print(seconds);
    
    
    // 10th of a second
    display.setTextSize(2);
    display.print(":");
    if (centiseconds < 10) display.print(0);
    display.println(centiseconds);
    
    display.display();
  }
}

