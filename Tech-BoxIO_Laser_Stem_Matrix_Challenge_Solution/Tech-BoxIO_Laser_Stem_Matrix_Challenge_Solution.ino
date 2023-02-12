//Start by including the needed library
#include "src/FastLED.h"

FASTLED_USING_NAMESPACE

//Define the following variables
#define DATA_PIN    7         //Declare the output pin
#define PB_pin      13        //Declare the input pin
#define LED_TYPE    WS2812B   //Set the LED type
#define COLOR_ORDER GRB       //Set the LED Color Order type - Green, Red, Blue
#define NUM_LEDS    64        //Set the number of LEDs appearing in the LED Matrix
#define BRIGHTNESS  100       //Set the base brightness of the LEDs

//Initialize the leds object with the correct number of leds appearing in the matrix
CRGB leds[NUM_LEDS];


void setup() {
  delay(1000); //1 second delay for recovery

  pinMode(DATA_PIN, OUTPUT);
  pinMode(PB_pin, INPUT);

  //Set the FastLED configuration for the LED Matrix being used
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  //Set the master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


//List of patterns that cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, chasing, juggle, twinkle, bpm, animatedHeart };

uint8_t FRAMES_PER_SECOND = 60;     //Set a modest framerate
uint8_t gCurrentPatternNumber = 0;  //Index number of which pattern is current
uint8_t gHue = 0;                   //Rotating "base color" used by many of the patterns
uint8_t buttonState = 0;            //Variable for reading the button state
uint8_t lastButtonState = 0;        //Variable for holding the previous button state
uint8_t mode = 0;                   //Variable for changing the mode of operation
// 0 - Demo Mode: Rotates through all functions
// 1 - Rainbow
// 2 - Rainbow with Glitter
// 3 - Confetti 
// 4 - Chasing
// 5 - Juggle 
// 6 - Twinkle
// 7 - BPM: Beats per Minute
uint8_t numModes = 9;               //Define the number of modes


void loop()
{
  buttonState = digitalRead(PB_pin);    //Read the button state and assign it to the variable

  if(buttonState != lastButtonState)    //If the button state has changed, then increment the current mode if the new button state is 'HIGH'
  {
    if(buttonState == HIGH)
    {
      mode = (mode + 1) % numModes;
    }
    delay(50);                          //Insert a debounce delay
  }
  lastButtonState = buttonState;        //Set current button state to last button state

  if(mode == 0) { //Set the frames per second based on the active mode if set to Demo mode (0)
    if(gCurrentPatternNumber == (0 || 1)) { //If in modes 0 or 1, set a faster framerate of 120 fps
      FRAMES_PER_SECOND = 120;    
    }
    else if(gCurrentPatternNumber == (5)) { //If in mode 5, set a slow framerate of 15 fps
      FRAMES_PER_SECOND = 15;
    }
    else {
      FRAMES_PER_SECOND = 60; //Otherwise, maintain a modest framerate of 60 fps
    }

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber](); 

    EVERY_N_SECONDS( 10 ) { nextPattern(); } //Change the current pattern periodically
  }
  else {  //Otherwise, set the active pattern to the mode value - 1
    gPatterns[mode - 1]();
  }

  
  FastLED.show();  //Display the LED array values on the LED matrix
  FastLED.delay(1000/FRAMES_PER_SECOND);  //Insert the framerate delay to keep the framerate modest

  //Do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } //Slowly cycle the 'base color' through the rainbow
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  //Add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  //Built-in FastLED rainbow, plus random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  //Add random glitter by setting random LEDs to white
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  //Random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);   //Slowly fade the illuminated LEDs
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);   //Assign a random LED a random color
}

void chasing()
{
  //A colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);   //Slowly fade the illuminated LEDs
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);   //Assign the LED at the pos position the CHSV value
}

void bpm()
{
  //Colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;    //Use the built-in color palette
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //For each led, assign a color from the palette based on the hue and beat
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  //Eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);   //Slowly fade the illuminated LEDs
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {   //Assign a CHSV value with hue based on dothue and timed with the beat
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void twinkle() {
  //LED Matrix twinkles white with random speckles/glitter
  fadeToBlackBy( leds, NUM_LEDS, 30);   //Slowly fade the illuminated LEDs
  int pos = random16(NUM_LEDS);   //Select an LED position randomly
  leds[pos] += CRGB::White;   //Assign the randomly selected LED the White value
}


// --- Challenge Solution ---
// Create a frame variable to alternate between images
bool frame = false;

// Array for the Small Heart Image
static uint8_t heart_small[NUM_LEDS] ={
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0, 
  0,  0,  1,  1,  1,  1,  0,  0,
  0,  0,  1,  1,  1,  1,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
};

// Array for the Largest Heart Image
static uint8_t heart_large[NUM_LEDS] = {
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  1,  1,  0,  0,  1,  1,  0,
  1,  1,  1,  1,  1,  1,  1,  1, 
  1,  1,  1,  1,  1,  1,  1,  1,
  0,  1,  1,  1,  1,  1,  1,  0,
  0,  0,  1,  1,  1,  1,  0,  0, 
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

// Create a function to display the beating heart frames
void animatedHeart() 
{
  FastLED.clear();  //Clear the LED Array
  
  //A heart displays and beats across the Laser Stems
  for (int i = 0; i < NUM_LEDS; i++){ // Cycle through each LED on the matrix
    if (frame == false) { // If the small frame is next, load it into leds[]
      if (heart_small[i] == 1) {
        leds[i] += CRGB::Red;
      }
    }
    else {  // Else, if the large frame is next, load it into leds[]
      if (heart_large[i] == 1) {
        leds[i] += CRGB::Red;
      }
    }
  }
  delay(400);
  frame = !frame; // Alternate between frames
}
