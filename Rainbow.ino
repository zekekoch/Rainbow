
#include <OctoWS2811.h>
#include <Wire.h>

const int ledsPerStrip = 150;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int rainbowColors[180];


void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  digitalWrite(1, LOW);
  leds.begin();
  Wire.begin(47);
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);
}

byte mode  = 0;

byte emissionState;
byte cutOffState;
byte intensifierState;
byte lifeState;
byte dialState;

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  if (howMany == 2)
  {
    byte which = Wire.read(); // receive byte as a character
    byte value = Wire.read();

    switch(which)
    {
      case 'E':
        emissionState = value;
        break;
      case 'C':
        cutOffState = value;
        break;
      case 'I':
        intensifierState = value;
        break;
      case 'L':
        lifeState = value;
        break;
      case 'D':
        dialState = value;
        break;
    }
  }
}

void printState()
{
  Serial.print("E");
  Serial.print(emissionState);
  Serial.print("C");
  Serial.print(cutOffState);
  Serial.print("I");
  Serial.print(intensifierState);
  Serial.print("L");
  Serial.print(lifeState);
  Serial.print("D");
  Serial.print(dialState);
  Serial.println();
}

void loop() {

  rainbow(dialState * 20, cutOffState * 2);
  printState();
}


// phaseShift is the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift, int cycleTime)
{
  int color, x, y, offset, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 180; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x + y*phaseShift/2) % 180;
        leds.setPixel(x + y*ledsPerStrip, rainbowColors[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}