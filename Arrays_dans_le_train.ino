/*
 * This is a temporary sketch to understand needed code to build a simple Midi Controller which later will be used as an Adobe Ligthroom Controller.
last modified on 17 Jun 2017
by midijon

All buttons will be implemented using Shift Registers (595 type) or Muxes (4067 type).
I decided not to use bounce.h for now, and I have absolutely no idea why (maybe because I need to understand a bit better the way it works before)

This sketch includes:
• 1 Teensy 3.6
• 16 Rotary Encoders KY-040 type
• 5 LEDs
• 3 Buttons

Will be added later :
• 1 additional Rotary Encoder
• 21 additional Buttons
• possibly 8 RGB leds
• possibly a 2x16 LCD screen to retrieve MIDI Values from Adobe Lightroom (not in 1st version of build)

 */

#include <Encoder.h>

const int ShiftPin = 0;   // This is the button to activate Alternate action for buttons and 8 first encoders
const int hslPin = 35;    // This is the button to switch from HUE, to SAT, to LUM, will be shifted or muxed
const int bwPin = 36;     // This is the button to switch from Color to BW, will be shifted or muxed
const int h_ledPin = 36;  // This LED when HIGH shows the HUE mode is selected
const int s_ledPin = 37;  // This LED when HIGH shows the SAT mode is selected
const int l_ledPin = 38;  // This LED when HIGH shows the LUM mode is selected
const int bw_ledPin = 39; // This LED when HIGH shows the B&W mode is selected
const int ledPin = 32;    // This LED when HIGH shows the Shift Button is pressed
int ShiftState = 0;
int bwPinState = 0;
int hslPinState = 0;
int hslStatus = 0;
const int hue = 0;
const int sat = 1;
const int lum = 2;
const int bw = 3;
int delayhsl = 100;       //TBD depending on how sensitive it is, no need to be fast as button is only "slowly" clicked
const int bwNote = 127;   // This is the Note# assigned to the Color/B&W switch function

// What follow is the list of all encoders, there are 16 in this sketch
// This amount is subject to variation, and for now, no encoder is shifted or muxed

Encoder Enc1(1, 2);
int value1;
Encoder Enc2(3, 4);
int value2;
Encoder Enc3(5, 6);
int value3;
Encoder Enc4(7, 8);
int value4;
Encoder Enc5(9, 10);
int value5;
Encoder Enc6(11, 12);
int value6;
Encoder Enc7(14, 15);
int value7;
Encoder Enc8(16, 17);
int value8;
Encoder Enc9(18, 19);
int value9;
Encoder Enc10(20, 21);
int value10;
Encoder Enc11(22, 23);
int value11;
Encoder Enc12(24, 25);
int value12;
Encoder Enc13(26, 27);
int value13;
Encoder Enc14(28, 29);
int value14;
Encoder Enc15(30, 31);
int value15;
Encoder Enc16(33, 34);
int value16;

long Enc1_previous = -999;      // I've no fucking idea what this is
long Enc2_previous = -999;      // I've no fucking idea what this is
long Enc3_previous = -999;      // I've no fucking idea what this is
long Enc4_previous = -999;      // I've no fucking idea what this is
long Enc5_previous = -999;      // I've no fucking idea what this is
long Enc6_previous = -999;      // I've no fucking idea what this is
long Enc7_previous = -999;      // I've no fucking idea what this is
long Enc8_previous = -999;      // I've no fucking idea what this is
long Enc9_previous = -999;      // I've no fucking idea what this is
long Enc10_previous = -999;     // I've no fucking idea what this is
long Enc11_previous = -999;     // I've no fucking idea what this is
long Enc12_previous = -999;     // I've no fucking idea what this is
long Enc13_previous = -999;     // I've no fucking idea what this is
long Enc14_previous = -999;     // I've no fucking idea what this is
long Enc15_previous = -999;     // I've no fucking idea what this is
long Enc16_previous = -999;     // I've no fucking idea what this is

const int MIDI_CC_ENC[8] = {66, 67, 68, 69, 70, 71, 72, 73};            // Those are the 8 main encoders CC#
const int MIDI_CC_ENC_SHIFT[8] = {74, 75, 76, 77, 78, 79, 80, 81};      // Those are the 8 main encoders CC# when Shift button is pressed
const int MIDI_CC_ENC_hue[8] = {82, 83, 84, 85, 86, 87, 88, 89};        // Those are the 8 HSL encoders CC# in HUE mode
const int MIDI_CC_ENC_sat[8] = {90, 91, 92, 93, 94, 95, 96, 97};        // Those are the 8 HSL encoders CC# in SAT mode
const int MIDI_CC_ENC_lum[8] = {98, 99, 100, 101, 102, 103, 104, 105};  // Those are the 8 HSL encoders CC# in LUM mode
const int MIDI_CC_ENC_bw[8] = {106, 107, 108, 109, 110, 111, 112, 113}; // Those are the 8 HSL encoders CC# in B&W mode
int MIDI_CC_ENC_value[8] = {value1, value2, value3, value4, value5, value6, value7, value8};
int MIDI_CC_ENC_hsl_value[8] = {value9, value10, value11, value12, value13, value14, value15, value16};
int ENC_PREVIOUS[8] = {Enc1_previous, Enc2_previous, Enc3_previous, Enc4_previous, Enc5_previous, Enc6_previous, Enc7_previous, Enc8_previous};
int ENC_PREVIOUS_hsl[8] = {Enc9_previous, Enc10_previous, Enc11_previous, Enc12_previous, Enc13_previous, Enc14_previous, Enc15_previous, Enc16_previous};


// Setup only sets INPUT or OUPUT mode for every button and/or LED and selects the HUE mode as default when plugging

void setup() {

  pinMode(ShiftPin, INPUT);
  pinMode(ShiftPin, INPUT_PULLUP);
  pinMode(hslPin, INPUT);
  pinMode(hslPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin, INPUT_PULLUP);
  pinMode(h_ledPin, OUTPUT);
  pinMode(h_ledPin, INPUT_PULLUP);
  pinMode(s_ledPin, OUTPUT);
  pinMode(s_ledPin, INPUT_PULLUP);
  pinMode(l_ledPin, OUTPUT);
  pinMode(l_ledPin, INPUT_PULLUP);
  pinMode(bw_ledPin, OUTPUT);
  pinMode(bw_ledPin, INPUT_PULLUP);
  pinMode(bwPin, INPUT);
  pinMode(bwPin, INPUT_PULLUP);

  digitalWrite(h_ledPin, HIGH);
  digitalWrite(s_ledPin, LOW);
  digitalWrite(l_ledPin, LOW);
  digitalWrite(bw_ledPin, LOW);
}

void loop() {

  hslPinState = digitalRead(hslPin);
  delay(delayhsl);

  if (hslPinState == HIGH && hslStatus == hue)      // This switches from HUE mode to SAT mode
  {
    hslStatus = sat;                                // This selects the SAT mode
    digitalWrite(h_ledPin, LOW);                    // This turns the HUE led off to indicate it is no more the selected mode
    digitalWrite(s_ledPin, HIGH);                   // This turns the SAT led on to indicate is is the selected mode
  }

  if (hslPinState == HIGH && hslStatus == sat)      // This switches from SAT mode to LUM mode
  {
    hslStatus = lum;                                // This selects the LUM mode
    digitalWrite(s_ledPin, LOW);                    // This turns the SAT led off to indicate it is no more the selected mode
    digitalWrite(l_ledPin, HIGH);                   // This turns the LUM led on to indicate is is the selected mode
  }

  if (hslPinState == HIGH && hslStatus == lum)      // This switches from LUM mode to HUE mode
  {
    hslStatus = hue;                                // This selects the HUE mode
    digitalWrite(l_ledPin, LOW);                    // This turns the LUM led off to indicate it is no more the selected mode
    digitalWrite(h_ledPin, HIGH);                   // This turns the HUE led on to indicate is is the selected mode
  }

  bwPinState = digitalRead(bwPin);
  delay(delayhsl);

  if (bwPin == HIGH)                                  // This is what happens when the Color/B&W button is pressed
  {
    if (bw_ledPin == LOW)                             // We use the LED to determine if B&W mode is selected or not
    {
      hslStatus = bw;                                 // This selects the B&W mode
      digitalWrite(h_ledPin, LOW);                    // This turns the HUE led off to indicate it is no more the selected mode
      digitalWrite(s_ledPin, LOW);                    // This turns the SAT led off to indicate is no more the selected mode
      digitalWrite(l_ledPin, LOW);                    // This turns the LUM led off to indiate it is no more the selected mode
      digitalWrite(bw_ledPin, HIGH);                  // This turns the B&W led on to indicate it is the selected mode
      usbMIDI.sendNoteOff(bwNote, 0, 1);             // This activates B&W mode within LightRoom (switches from Color (default) to BW)
    }
    else if (bw_ledPin == HIGH)                       // We use the LED to determine if B&W mode is selected or not
    {
      hslStatus = hue;                                // This selects the HUE mode
      digitalWrite(h_ledPin, HIGH);                   // This turns the HUE led on to indicate it is now selected again
      digitalWrite(s_ledPin, LOW);                    // This turns the SAT led off to indicate is no more the selected mode
      digitalWrite(l_ledPin, LOW);                    // This turns the LUM led off to indiate it is no more the selected mode
      digitalWrite(bw_ledPin, LOW);                   // This turns the B&W led on to indicate it is the selected mode
      usbMIDI.sendNoteOn(bwNote, 110, 1);             // This activates Color mode within LightRoom (switches from BW to Color)
    }
  }

  ShiftState = digitalRead(ShiftPin);                 // This is our Shift button function indicator, turns LED On or Off depending if it is pressed
  delay(10);
  if (ShiftState == HIGH)
  {
    digitalWrite(ledPin, HIGH );
  }
  else if (ShiftState == LOW)
  {
    digitalWrite(ledPin, LOW );
  }

// I didn't find a way to use arrays for this, as it uses the .read() and .write() functions, it doesn't let me read within an array
// Any help is welcome on this one, at least just for my understanding

  value1 = Enc1.read();
  if (value1 > 127) {
    Enc1.write(127);
  }
  else if (value1 < 0) {
    Enc1.write(0);
  }

  value2 = Enc2.read();
  if (value2 > 127) {
    Enc2.write(127);
  }
  else if (value2 < 0) {
    Enc2.write(0);
  }

  value3 = Enc3.read();
  if (value3 > 127) {
    Enc3.write(127);
  }
  else if (value3 < 0) {
    Enc3.write(0);
  }

  value4 = Enc4.read();
  if (value4 > 127) {
    Enc4.write(127);
  }
  else if (value4 < 0) {
    Enc4.write(0);
  }

  value4 = Enc4.read();
  if (value4 > 127) {
    Enc4.write(127);
  }
  else if (value4 < 0) {
    Enc4.write(0);
  }

  value5 = Enc5.read();
  if (value5 > 127) {
    Enc5.write(127);
  }
  else if (value5 < 0) {
    Enc5.write(0);
  }

  value6 = Enc6.read();
  if (value6 > 127) {
    Enc6.write(127);
  }
  else if (value6 < 0) {
    Enc6.write(0);
  }

  value7 = Enc7.read();
  if (value7 > 127) {
    Enc7.write(127);
  }
  else if (value7 < 0) {
    Enc7.write(0);
  }

  value8 = Enc8.read();
  if (value8 > 127) {
    Enc8.write(127);
  }
  else if (value8 < 0) {
    Enc8.write(0);
  }

  value9 = Enc9.read();
  if (value9 > 127) {
    Enc9.write(127);
  }
  else if (value9 < 0) {
    Enc9.write(0);
  }

  value10 = Enc10.read();
  if (value10 > 127) {
    Enc10.write(127);
  }
  else if (value10 < 0) {
    Enc10.write(0);
  }

  value11 = Enc11.read();
  if (value11 > 127) {
    Enc11.write(127);
  }
  else if (value11 < 0) {
    Enc11.write(0);
  }

  value12 = Enc12.read();
  if (value12 > 127) {
    Enc12.write(127);
  }
  else if (value12 < 0) {
    Enc12.write(0);
  }

  value13 = Enc13.read();
  if (value13 > 127) {
    Enc13.write(127);
  }
  else if (value13 < 0) {
    Enc13.write(0);
  }

  value14 = Enc14.read();
  if (value14 > 127) {
    Enc14.write(127);
  }
  else if (value14 < 0) {
    Enc14.write(0);
  }

  value1 = Enc1.read();
  if (value1 > 127) {
    Enc1.write(127);
  }
  else if (value1 < 0) {
    Enc1.write(0);
  }

  value15 = Enc15.read();
  if (value15 > 127) {
    Enc15.write(127);
  }
  else if (value15 < 0) {
    Enc15.write(0);
  }

  value16 = Enc16.read();
  if (value16 > 127) {
    Enc16.write(127);
  }
  else if (value16 < 0) {
    Enc16.write(0);
  }


  for (int i = 0; i < 8; i++)                                                       // This scans our 8 main encoders in loop
  {
    if (ShiftState == HIGH)                                                         // This is what happens if SHIFT is pressed
    {
      MIDI_CC_ENC_value[i] = constrain(MIDI_CC_ENC_SHIFT[i], 0, 127);
      if (MIDI_CC_ENC_value[i] != ENC_PREVIOUS[i]) {
        ENC_PREVIOUS[i] = MIDI_CC_ENC_value[i];
        usbMIDI.sendControlChange(MIDI_CC_ENC_SHIFT[i], MIDI_CC_ENC_value[i], 1);
      }
    }
    else if (ShiftPin == LOW)                                                       // This is what happens if SHIFT is not pressed
    {
      MIDI_CC_ENC_value[i] = constrain(MIDI_CC_ENC_value[i], 0, 127);
      if (MIDI_CC_ENC_value[i] != ENC_PREVIOUS[i]) {
        ENC_PREVIOUS[i] = MIDI_CC_ENC_value[i];
        usbMIDI.sendControlChange(MIDI_CC_ENC[i], MIDI_CC_ENC_value[i], 1);
      }
    }
  }

  for (int i = 0; i < 8; i++)                                                       // This scans our 8 HSL encoders in loop
  {
    if (hslStatus == hue)                                                           // This is what happens if we are in HUE mode
    {
      MIDI_CC_ENC_hsl_value[i] = constrain(MIDI_CC_ENC_hsl_value[i], 0, 127);
      if (MIDI_CC_ENC_hsl_value[i] != ENC_PREVIOUS_hsl[i]) {
        ENC_PREVIOUS_hsl[i] = MIDI_CC_ENC_hsl_value[i];
        usbMIDI.sendControlChange(MIDI_CC_ENC_hue[i], MIDI_CC_ENC_hsl_value[i], 1);
      }
    }
    if (hslStatus == sat)                                                           // This is what happens if we are in SAT mode
    {
      MIDI_CC_ENC_hsl_value[i] = constrain(MIDI_CC_ENC_hsl_value[i], 0, 127);
      if (MIDI_CC_ENC_hsl_value[i] != ENC_PREVIOUS_hsl[i]) {
        ENC_PREVIOUS_hsl[i] = MIDI_CC_ENC_hsl_value[i];
        usbMIDI.sendControlChange(MIDI_CC_ENC_sat[i], MIDI_CC_ENC_hsl_value[i], 1);
      }

      if (hslStatus == lum)                                                         // This is what happens if we are in LUM mode
      {
        MIDI_CC_ENC_hsl_value[i] = constrain(MIDI_CC_ENC_hsl_value[i], 0, 127);
        if (MIDI_CC_ENC_hsl_value[i] != ENC_PREVIOUS_hsl[i]) {
          ENC_PREVIOUS_hsl[i] = MIDI_CC_ENC_hsl_value[i];
          usbMIDI.sendControlChange(MIDI_CC_ENC_lum[i], MIDI_CC_ENC_hsl_value[i], 1);
        }

        if (hslStatus == bw)                                                        // This is what happens if we are in BW mode
        {
          MIDI_CC_ENC_hsl_value[i] = constrain(MIDI_CC_ENC_hsl_value[i], 0, 127);
          if (MIDI_CC_ENC_hsl_value[i] != ENC_PREVIOUS_hsl[i]) {
            ENC_PREVIOUS_hsl[i] = MIDI_CC_ENC_hsl_value[i];
            usbMIDI.sendControlChange(MIDI_CC_ENC_bw[i], MIDI_CC_ENC_hsl_value[i], 1);
          }
        }
      }
    }
  }
}
