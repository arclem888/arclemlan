
// stm8 coin sensor for arclem pisowifi
#define I2C_SLAVE_ADDRESS 15
#include <EEPROM.h>
#include <Wire.h>

#define COIN_ADDRESS 0
#define COINPIN 4
#define COINLED 6
#define RELAYPIN 5
#define COINDELAY 10

#define RELAY_ON 7
#define RELAY_OFF 8
#define CLEAR 9

int coincounter = 0;
int coinState = 0;         // current state of the button
int lastCoinState = 0;     // previous state of the button



void setup() {
  // put your setup code here, to run once:
  pinMode(COINPIN, INPUT_PULLUP);
  pinMode(RELAYPIN, OUTPUT);
  pinMode(COINLED, OUTPUT);
  digitalWrite(COINLED, HIGH);
 
  delay(500); // wait for 500 milliseconds for coinslot to boot up
  coincounter = EEPROM.read(COIN_ADDRESS);
  if (coincounter == 255 ) {
    coincounter = 0;
    EEPROM.update(COIN_ADDRESS, coincounter); 
  }
  Wire.begin(I2C_SLAVE_ADDRESS);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
}


void checkCoinAndSave() {
  coinState = digitalRead(COINPIN);
  // compare the buttonState to its previous state
  if (coinState != lastCoinState) {
    // if the state has changed, increment the counter
    if (coinState == LOW) {
      // if the current state is HIGH then the button went from off to on:
      coincounter++;
      EEPROM.update(COIN_ADDRESS, coincounter);
    }
    // Delay a little bit to avoid bouncing
    delay(COINDELAY);
    lastCoinState = coinState;
    checkCoinAndSave();
  }
  // save the current state as the last state, for next time through the loop
  lastCoinState = coinState;
}

void requestEvent()
{
  Wire.write(coincounter);
}

void receiveEvent(int howMany) {

  int r = Wire.read();
 
  if (r == RELAY_ON) {
    digitalWrite(RELAYPIN, LOW);
    digitalWrite(COINLED, LOW);
  } else if (r == RELAY_OFF) {
    digitalWrite(RELAYPIN, HIGH);
    digitalWrite(COINLED, HIGH);
  } else if ( r == CLEAR) {
    coincounter = 0;
    EEPROM.update(COIN_ADDRESS, coincounter);
  }
  
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    // empty wire
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  checkCoinAndSave();
  delay(1);
}
