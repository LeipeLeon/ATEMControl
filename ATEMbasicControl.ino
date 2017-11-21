/*****************
 * Example: ATEM Basic Control
 * Connects to the Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
 * Uses digital inputs 2 and 3 (active High) to select input 1 and 2 on Preview Bus
 * Uses digital input 7 (active High) to "Cut" (Preview and Program swaps)
 * Uses digital outputs 4 and 5 for Tally LEDs for input 1 and 2 (Active LOW)
 * See file "Breadboard circuit for ATEM basic control.pdf" for suggested breadboard schematics (find it under ATEM/examples/ATEMbasicControl/ in this library)
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0x6A, 0x84 };      // <= SETUP!  MAC address of the Arduino
IPAddress clientIp(192, 168, 3, 253);        // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 3, 254);      // <= SETUP!  IP address of the ATEM Switcher

// Include ATEM library and make an instance:
#include <ATEM.h>

// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
ATEM AtemSwitcher(IPAddress(192, 168, 3, 254), 56417);  // <= SETUP (the IP address of the ATEM switcher)

int camPins[] = {2,3,4,8,9};

void setup() {

  // Set up pins for
  pinMode(2, INPUT);  // Cam 1
  pinMode(3, INPUT);  // Cam 2
  pinMode(4, INPUT);  // Cam 3
  pinMode(5, INPUT);  // Cut
  pinMode(6, INPUT);  // Auto
  pinMode(7, INPUT);  // CTRL
  pinMode(8, INPUT);  // Cam 4
  pinMode(9, INPUT);  // Cam 5
  // pinMode(4, OUTPUT);  // LED: 1 is on Program (Tally)
  // pinMode(5, OUTPUT);  // LED: 2 is on Program (Tally)

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,clientIp);
  Serial.begin(9600);

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

// For pushButtons:
int pushButton = 0;
int transBtn = 0;
int ctrlBtn = 0;
bool ctrl = false;

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  // // Write the Tally LEDS:
  // digitalWrite(4, !AtemSwitcher.getPreviewTally(1));
  // digitalWrite(5, !AtemSwitcher.getPreviewTally(2));

  for (int i=0; i <= sizeof(camPins); i++){
    if (digitalRead(camPins[i]))  {
      if (pushButton != i)  {
        pushButton = i;
        Serial.print("Select ");
        Serial.print(i+1, DEC);
        if (ctrl) {
          Serial.println(" to Preview");
          // AtemSwitcher.changePreviewInput(i);
        } else {
          Serial.println(" to Program");
          // AtemSwitcher.changeProgramInput(i);
        }
      }
    }
  }
  if (digitalRead(5) && transBtn != 1) {
    Serial.println("Cut");
    // AtemSwitcher.doCut();
    transBtn = 1;
  } else if (digitalRead(6) && transBtn != 2){
    Serial.println("Auto");
    // AtemSwitcher.doAuto();
    transBtn = 2;
  } else {
    transBtn = 0;
  }
  if (digitalRead(7) && ctrlBtn != 1) {
    ctrl = !ctrl;
    if (ctrl) {
      Serial.println("CTRL 1");
    } else {
      Serial.println("CTRL 0");
    }
    ctrlBtn = 1;
    // Write status to LED
  } else {
    ctrlBtn = 0;
  }
}
