
/*************************************************
* Public Constants
*************************************************/

// NOTE FREQUENCIES
#define  C3  130.81
#define Db3 138.59
#define D3  146.83
#define Eb3 155.56
#define E3  164.81
#define F3  174.61
#define Gb3 185.00
#define G3  196.00
#define Ab3 207.65
#define LA3 220.00
#define Bb3 233.08
#define B3  246.94
#define C4  261.63
#define Db4 277.18
#define D4  293.66
#define Eb4 311.13
#define E4  329.63
#define F4  349.23
#define Gb4 369.99
#define G4  392.00
#define Ab4 415.30
#define LA4 440.00
#define Bb4 466.16
#define B4  493.88
#define C5  523.25
#define Db5 554.37
#define D5  587.33
#define Eb5 622.25
#define E5  659.26
#define F5  698.46
#define Gb5 739.99
#define G5  783.99
#define Ab5 830.61
#define LA5 880.00
#define Bb5 932.33
#define B5  987.77
 
// DURATION OF THE NOTES 
#define BPM 240     //you can change this value changing all the others
#define Q 60000/BPM //quarter 1/4 
#define H 2*Q       //half 2/4
#define T 3*Q       //three quarter 3/4
#define E Q/2       //eighth 1/8
#define S Q/4       // sixteenth 1/16
#define W 4*Q       // whole 4/4
 
// CHECKS FOR BUTTON AND LIGHT POSITIONS
#define CHOICE_OFF    0 //Used to control LEDs
#define CHOICE_NONE   0 //Used to check buttons
#define CHOICE_RED    (1 << 0)
#define CHOICE_GREEN  (1 << 1)
#define CHOICE_BLUE   (1 << 2)
#define CHOICE_YELLOW (1 << 3)
 
// DEFINE PIN LOCATIONS
#define LED_RED       A0 //Nää on nyt värin määritykseen käytettävät
#define LED_GREEN     A1 // punanen default
#define LED_BLUE      A2
#define LED_YELLOW    A3
#define BUTTON_RED    9 //valintanapit
#define BUTTON_GREEN  10
#define BUTTON_BLUE   11
#define BUTTON_YELLOW 12

#define BUTTON_CHEAT A4 //loser nappi
#define BUTTON_RESET BUTTON_CHEAT //reset

#define BUZZER        6
#define SERVOPIN      8

//ULTRAÄÄNI ETÄISYYSSENSORI
#define ECHOPIN 8
#define TRIGPIN 7

//RAMPIN LINEAARIMOOTTORI
int enA = 2; //Moottori enable DC
int in1 = 3; //Moottori ohjauspinni (PWM)
int in2 = 4; //Moottori ohjauspinni

//PAINIKKEIDEN LED
int enB = 13; //LED valoteho, analog säätö 0-255
int in3 = 5; //LED ohjauspinni ON/OFF
int LED_laskuri;
int LED_laskuri_kerroin = 4; //Määrä kuinka monta kertaa LED vilkkuvat ajan loputtua
 
//PARAMETRIT
#define ENTRY_TIME_LIMIT 30000 //Aika jonka peli odottaa vastausta ennen ajan loppumista
#define RAPID_FLASH_TIME 15000
#define DIST_TRESHOLD 1200
#define BUTTON_COLOR_FREQ 150
#define RAMP_TIME 3000

 
void setup() // Run once when power is connected
{
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  analogWrite(enB, 255);
    
  pinMode(BUZZER, OUTPUT);
  
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  
  pinMode(BUTTON_CHEAT, INPUT_PULLUP);
  pinMode(BUTTON_RESET, INPUT_PULLUP);
 
  pinMode(LED_RED, INPUT_PULLUP);
  pinMode(LED_GREEN, INPUT_PULLUP);
  pinMode(LED_BLUE, INPUT_PULLUP);
  pinMode(LED_YELLOW, INPUT_PULLUP);

  pinMode(TRIGPIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHOPIN, INPUT); // Sets the echoPin as an Input

  //Serial.begin(9600);


  // Reset ramp to low
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW); 
  
  analogWrite(enA, 255);  
  delay(1000);
  analogWrite(enA, 0);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

}
 
 
void loop()
{
  
  int cheated = attractMode(); // Blink lights while waiting for user to press a button
  
  if (cheated || play_memory()) 
    play_winner(); // Player won, play winner tones
  else 
    play_loser(); // Player lost, play loser tones
  
}
 
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//THE FOLLOWING FUNCTIONS CONTROL GAME PLAY
 
// Returns 0 if player loses, or 1 if player wins
boolean play_memory(void)
{
  play(C5,  100);
  delay(33);
  play(C5,  100);
  delay(33);
  play(C5,  100);
  delay(33);
  play(C5,  100);
  
  int correct_button = CHOICE_YELLOW;
  if(!digitalRead(LED_GREEN))
    correct_button = CHOICE_GREEN;
  else if(!digitalRead(LED_BLUE))
    correct_button = CHOICE_BLUE;
  else if(!digitalRead(LED_RED))
    correct_button = CHOICE_RED;
  
  return wait_for_button() == correct_button;
}
 
 
// Wait for a button to be pressed. 
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out

byte wait_for_button(void)
{
  long startTime = millis(); // Remember the time we started the this loop
  long t = 0;

  do
  {
    t = millis() - startTime;
    byte button = checkButton();
 
    if (button != CHOICE_NONE)
    { 
      toner(button, Q); // Play the button the user just pressed
      while(checkButton() != CHOICE_NONE) delay(1);  // Now let's wait for user to release button
      delay(10); // This helps with debouncing and accidental double taps
      return button;
    }
    delay(1);

    //Vilkutetaan nappien LED, variable State ja aikamääreen avulla
    int button_light = (t / BUTTON_COLOR_FREQ) % 6;
    if(button_light == 0 || button_light == 2 || (button_light == 4 && millis() - startTime > RAPID_FLASH_TIME))
    {
      digitalWrite(in3, HIGH); //Nappien LED päälle
    }
    else
    {
      digitalWrite(in3, LOW); //Nappien LED pois päältä
    }
    if(t > ENTRY_TIME_LIMIT - 2000)
      play(F5,  2);
  } while(t < ENTRY_TIME_LIMIT); // Loop until too much time has passed


  return CHOICE_NONE;   // VASTAUSAIKA LOPPUI
}
 
// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton(void)
{
  if (digitalRead(BUTTON_RED) == 0) return(CHOICE_RED); 
  else if (digitalRead(BUTTON_GREEN) == 0) return(CHOICE_GREEN); 
  else if (digitalRead(BUTTON_BLUE) == 0) return(CHOICE_BLUE); 
  else if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW);
 
  return(CHOICE_NONE); // If no button is pressed, return none
}
 
// Light an LED and play tone
// Red, upper left:     440Hz - A4
// Green, upper right:  880Hz - A5
// Blue, lower left:    587.33Hz - D5
// Yellow, lower right: 784Hz - G5
void toner(byte which, int dur)
{
 
  //Play the sound associated with the given LED
  switch(which) 
  {
  case CHOICE_RED:
    play(LA4, dur); 
    break;
  case CHOICE_GREEN:
    play(LA5, dur); 
    break;
  case CHOICE_BLUE:
    play(D5, dur); 
    break;
  case CHOICE_YELLOW:
    play(G5, dur); 
    break;
  }
 
}
 
 
// Play the winner sound and lights
void play_winner(void) //KUN KISAAJA NÄPPÄILEE OIKEIN, VOITTAA PELIN JA RAMPPI AUKEAA
{  
 
  play(F5,  W);
  play(E5,  Q);
  play(F5,  Q);
  play(E5,  Q);
  play(C5,  T);
  play(LA4, Q);
  play(D5,  H);
  play(LA4,  W); 
  
  open_ramp();
}
 
 
void open_ramp(void)
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  analogWrite(enA, 255);
  delay(RAMP_TIME);
  analogWrite(enA, 0);

  //WAIT FOR RESET BUTTON
  while(digitalRead(BUTTON_RESET)) delay(1);
  
  // now change motor directions
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW); 
  
  analogWrite(enA, 255);  
  delay(RAMP_TIME);
  analogWrite(enA, 0);
  // now turn off motors
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
 

void play_loser(void) // KUN KISAAJA NÄPPÄILEE VÄÄRIN JA HÄVIÄÄ PELIN
{
   for (LED_laskuri = 0; LED_laskuri < LED_laskuri_kerroin; LED_laskuri++) //Nappien LED vilkkuu häviön merkiksi
  {
    digitalWrite(in3, LOW);
    play(E3,  200);
    digitalWrite(in3, HIGH);
    play(C3,  200);
    delay(200);
    
  }
}
 
// Show an "attract mode" display while waiting for user to press button.
int attractMode(void)
{
  digitalWrite(in3, HIGH); //Nappien LED päälle
  
  // Play the ready sound
  play(C5,  100);
  play(E5,  100);
  
  // Wait until someone is close
  while(readDist() > DIST_TRESHOLD){
    if(!digitalRead(BUTTON_CHEAT)){
      digitalWrite(in3, LOW); //Nappien LED pois
      return 1;
    }
    delay(5);
  }
  digitalWrite(in3, LOW); //Nappien LED pois
  return 0;
}
 
int readDist(){
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long distance = pulseIn(ECHOPIN, HIGH);

  return distance;
}
 
 
void play(long note, int duration) {
  tone(BUZZER,note,duration);
  delay(1+duration);
}
