/*
  debug.h - Debug functions.
  Created by João Lino, September 25, 2015.
  Released into the public domain.
*/

#ifndef DEBUG_h
#define DEBUB_h

// ######################### TEMPLATES #########################
template <class T> void debugPrintVar( char *name, const T& value );
template <class T> void debugPrintVar( char *name, const T& value ) {
  Serial.print("[");
  Serial.print(name);
  Serial.print(":");
  Serial.print(value);
  Serial.println("]");
}
void debugPrintFunction( char *name ) {
  Serial.print("++++++++++++++++++++++++ ");
  Serial.print(name);
  Serial.println("++++++++++++++++++++++++");
}

#endif