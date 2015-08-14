#include "Arduino.h"
#include "Melody.h"

int simple_melody[] = {  _C,  _b,  _g,  _C,  _b,   _e,  _R,  _C,  _c,  _g, _a, _C };
int simple_tempo[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 };

int supermario_start_melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 };
int supermario_start_tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

int supermario_melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int supermario_tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};

//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

int song = 0;

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}

void sing(int s, int pin) {
  // iterate over the notes of the melody:
  song = s;

  switch(song) {
  	case 1: {
		int size = sizeof(supermario_melody) / sizeof(int);
	    for (int thisNote = 0; thisNote < size; thisNote++) {
	 
	      // to calculate the note duration, take one second
	      // divided by the note type.
	      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
	      int noteDuration = 1000 / supermario_tempo[thisNote];
	 
	      buzz(pin, supermario_melody[thisNote], noteDuration);
	 
	      // to distinguish the notes, set a minimum time between them.
	      // the note's duration + 30% seems to work well:
	      int pauseBetweenNotes = noteDuration * 1.30;
	      delay(pauseBetweenNotes);
	 
	      // stop the tone playing:
	      buzz(pin, 0, noteDuration);
	 
	    }
  		break;
  	}
  	case 2: {
		int size = sizeof(underworld_melody) / sizeof(int);
	    for (int thisNote = 0; thisNote < size; thisNote++) {
	 
	      // to calculate the note duration, take one second
	      // divided by the note type.
	      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
	      int noteDuration = 1000 / underworld_tempo[thisNote];
	 
	      buzz(pin, underworld_melody[thisNote], noteDuration);
	 
	      // to distinguish the notes, set a minimum time between them.
	      // the note's duration + 30% seems to work well:
	      int pauseBetweenNotes = noteDuration * 1.30;
	      delay(pauseBetweenNotes);
	 
	      // stop the tone playing:
	      buzz(pin, 0, noteDuration);
	 
	    }
  		break;
  	}
  	case 3: {
		int size = sizeof(supermario_start_melody) / sizeof(int);
	    for (int thisNote = 0; thisNote < size; thisNote++) {
	 
	      // to calculate the note duration, take one second
	      // divided by the note type.
	      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
	      int noteDuration = 1000 / supermario_start_tempo[thisNote];
	 
	      buzz(pin, supermario_start_melody[thisNote], noteDuration);
	 
	      // to distinguish the notes, set a minimum time between them.
	      // the note's duration + 30% seems to work well:
	      int pauseBetweenNotes = noteDuration * 1.30;
	      delay(pauseBetweenNotes);
	 
	      // stop the tone playing:
	      buzz(pin, 0, noteDuration);
	 
	    }
  		break;
  	}

  }
}