/*
      Group 17
      David Garcia
      Zachary Chua
      Deirdre Sheridan
      Gerry Migwi
*/

// LCD INITIALIZATION
#include <LiquidCrystal.h> // lcd library
LiquidCrystal lcd(4,5,6,7,8,12); // initialize the lcd

// INTERRUPT SECTION
// Here the variable used by the interrupt, ypos (position of the player vertically), is initialized as a volatile

volatile int ypos = 1; 

// CHARACTERS SECTION
// The characters drawn in the lcd are defined here

byte Player[] = {
  B00100,
  B01010,
  B00100,
  B11111,
  B00100,
  B00100,
  B01010,
  B10010
};

byte Rock[] = {
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111
};


// LED SECTION
// The section initializes the variables to make the LED blink between blue and green

int currentTime = 0; // For how much time has the program been running
int prevTime = 0; // For how much time the program ran when the LED changed its color
int colorPin = 10; // The pin in the arduino board that changes the voltage to change the color
int nocolorPin = 11; // The pin in the arduino board that changes the voltage to change the color


void setup() {

  Serial.begin(9600); // Start communication with the Arduino
  iniPins(); // Function to initialize the pins
  initializelcd(); // Function to initialize the lcd
  initialmessage(); // Function to display the initial message of the lcd
  attachInterrupt(digitalPinToInterrupt(2),moveup,CHANGE); // Definition of the interrupt moveup()
  randomSeed(analogRead(A1)); // Random generator seed 
  
  }



void loop() { 
  int note = 0; // Initialize the index of the song variable in the ledplay
  
  int xpos = 2; // Horizontal position of the player
  int rockpos; // Horizontal position of the rock
  
  int score = 0; // Score
  String strscore;  // Score (string)
  
  int elements; // Number of elements in the screen at a given point
  
  bool finish = true; // Boolean that stops the game

  // the while loop runs until the player has collided with a rock and the value of finish is changed to false
  
  while (finish) {
    
    int down = 0; // The amount of "time" the player has been in the air

    elements = random(1,5); // The amount of elements that are going to be on the screen
    int dbetween = random(4,8); // The difference between the first and second rock      
    rockpos = 15; // Starting position of the first rock
    bool collision = false; // If there has been a collision, it becomes true
    
    for (rockpos;rockpos+dbetween*(elements-1)>=0;rockpos--){
      
      printscore(strscore,score); // Prints the current score of the player
      ledplay(); // Blinks the RGB LED between blue and green
      note = playsong(note); // PLays the song and updates the next note that should be played
      
      // Draw the player
      lcd.setCursor(xpos,ypos);
      lcd.write(byte(0));

      // Draw the rocks
      for (int i = 0; i<elements;i++){
        if (rockpos+dbetween*i>=0 && rockpos+dbetween*i<=15){
          lcd.setCursor(rockpos+dbetween*i,1);
          lcd.write(byte(1));
        }
      }

      // Brings the player to the ground once the interrupt is pressed
      if(ypos==0){
        down++;
        if (down==3){
          ypos = 1;
          down = 0;
        }
      }

      // Determines if the player is in the same horizontal coordinate as the rock
      for (int i = 0; i<elements;i++){
        if (rockpos+dbetween*(i)==xpos){
          collision = true;
        }
      }

      // If the player is in the same horizontal coordinate, the conditional checks if it's 
      // in the same vertical and ends the game if it is. At the end changes the bool
      // collision to false.
      
      if (collision){
        if (ypos == 1){

          // Prints the results in the lcd
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.write("End!");
          lcd.setCursor(0,1);
          lcd.write("Score: ");
          lcd.setCursor(7,1);
          strscore = String(score);
          lcd.print(strscore);

          // Changes the color of the led
          ledloss();

          // Plays the song indefinitely
          while (true) {
            note = playsong(note);
            delay(300);
          }
        }
        collision = false;
        score = score + 20; // Updates the score
      }
      
      gamespeed(); // Delay determined by the potentiometer
      score = score + 5; // Updates the score
      lcd.clear(); // Clears the LCD
    }          
  }
}

void moveup(){
  // The interrupt changes the position of the player to make it jump
  ypos = 0; // Vertical position of the player is updated to move up
  
}

// Prints the score
void printscore(String strscore,int score){
  lcd.setCursor(13,0);
  strscore = String(score);
  lcd.print(strscore);
}

// Makes the LED blink between green and blue
int ledplay(){
  int intervalLED = 500;
  currentTime = millis();
  Serial.println(currentTime);
  Serial.println(prevTime);
  if (currentTime - prevTime < intervalLED){
    if (colorPin == 10){
      colorPin = 11;
      nocolorPin = 10;
    } else {
      colorPin = 10;
      nocolorPin = 11;   
    }
    analogWrite(9, 0);
    analogWrite(nocolorPin, 0);
    analogWrite(colorPin, 255);  
  }
  prevTime = currentTime;
}

// Changes the color of the LED to red when the user loses
void ledloss(){
  analogWrite(9,255);
  analogWrite(10,0);
  analogWrite(11,0);
}

// Initializes the Pins
void iniPins(){
  pinMode(1, OUTPUT);
  pinMode(2, INPUT);
  for (int i = 4;i>=12;i++){
    pinMode(i, OUTPUT);
  }
  digitalWrite(1,HIGH);
}

// Initialize the lcd
void initializelcd(){
  lcd.createChar(0, Player);
  lcd.createChar(1, Rock);
  lcd.clear();
  lcd.begin(16,2);
}

// Changes the speed at which the game is being played by using a delay; the value depends on a potentiometer
void gamespeed(){
  float voltage = analogRead(A0);
  voltage = voltage*5./1023.;
  float gamedelay = 125*voltage+200;
  delay(gamedelay);
  
}

// The initial message when the game starts
void initialmessage(){
  lcd.setCursor(0,0);
  lcd.print("Welcome! Press");
  lcd.setCursor(0,1);
  lcd.print("button to start");
  while (digitalRead(2)==LOW);
  lcd.clear();
}

// Plays the song
int playsong(int note){

  // Frequency of the notes
  int A = 440;
  int B = 493;
  int C = 523;
  int D1 = 294; 
  int D = 311;
  int E = 330;
  int F = 349;
  int G = 392;

  // Order of the notes
  int song[164] = {B, A, B, A, B, A, C, B, A, A, B, C, A, B, C, C, B, C, C, B, C, C, A, C, B, C, B, G, A, E, D, C, B, C, B,
  G, C, C, A, C, B, C, B, G, A, A, F, A, G, A, G, C, C, C, A, C, B, C, B, G, A, E, D, C, B, C, B, G, C, D, E, C, C, D,
  E, C, C, E, D1, C, D1, C, G, A, G, C, A, C, E, E, F, E, G, E, E, C, C, C, A, C, B, C, B, G, A, E, D, C, B, C, B, G,
  C, C, A, C, B, C, B, G, A, A, F, A, G, A, G, C, C, C, A, C, B, C, B, G, A, E, D, C, B, C, B, G, C, C, A, C, B, C, 
  B, G, A, A, A, F, A, G, A, G, C, C, E, D};
  
  int buzzerpin = 3; // Pin of the piezo
  noTone(buzzerpin); // Stops the previous note
  tone(buzzerpin,song[note%164]); // Plays the next note
  note++; // Updates the song[] index
  return note; // Returns the index
}
