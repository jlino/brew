/*
  Temperature.cpp - Library for measuring temperature with a Temperature.
  Created by João Lino, June 24, 2015.
  Released into the public domain.
*/
#include "Arduino.h"
#include "Temperature.h"

Temperature::Temperature(const char *name,
                    int OutputPin_SensorPower, 
                    int InputPin_TemperatureReading, 
                    int TimeBetweenReadings, 
                    float m1, 
                    float m2,
                    float b1,
                    float b2) {
    
    _name                              =   name;
    _OutputPin_SensorPower             =   OutputPin_SensorPower;
    _InputPin_TemperatureReading       =   InputPin_TemperatureReading;
    _TimeBetweenReadings               =   TimeBetweenReadings;
    _m1                                =   m1;
    _m2                                =   m2;
    _b1                                =   b1;
    _b2                                =   b2;

    _temperatureAverage                =   24.0;
    _measuredTemperature               =   24.0;
    _lastTemperatureRead               =   0;
    _VoutAnalogSample                  =   -1;
    _VoutPreviousAnalogSample          =   -1.0;
    _temperatureMeasurementsMarker     =   0;
    _rTemperatureMeasurementsMarker    =   0;
    _measuredTemperatureDeviation      =   0.0;
    _pump                              =   false;

    analogReference(INTERNAL1V1);                                   // EXTERNAL && INTERNAL2V56 && INTERNAL1V1
    pinMode(_OutputPin_SensorPower, OUTPUT);            // setup temperature sensor input pin
    digitalWrite(_OutputPin_SensorPower, LOW);      // initialize sensor on
}

void Temperature::safeHardwarePowerOff() {
  digitalWrite(_OutputPin_SensorPower, LOW);        // Turn temperature sensor OFF for safety
}

void Temperature::measure(boolean ln) {
  if(millis() - _lastTemperatureRead >= _TimeBetweenReadings) {                           //time to measure temperature
    
    /** measure Vout analog sample */
    digitalWrite(_OutputPin_SensorPower, HIGH);      // initialize sensor on
    delay(10);
    _VoutAnalogSample = analogRead(_InputPin_TemperatureReading);     // Get a reading
    digitalWrite(_OutputPin_SensorPower, LOW);      // initialize sensor on

    _lastTemperatureRead            = millis();                                             // Mark time of temperature reading

    _rTemperatureMeasurementsMarker++;                                                                                                                                                   // Position reading buffer marker at the last updated position
    if(_rTemperatureMeasurementsMarker >= TEMPERATURE_AVERAGE_VALUE_I) _rTemperatureMeasurementsMarker = 0;           // Check that it has not gone out of the buffer range
    _rTemperatureMeasurements[_rTemperatureMeasurementsMarker] = _VoutAnalogSample;
    
    //workingSample = GetMedianAverage(_rTemperatureMeasurements, 6);
    float workingSample = GetMedian(_rTemperatureMeasurements);
    /** Calculate temperature value */
    if(_pump) {
        _measuredTemperature = ( workingSample - _b1 ) / _m1;
    } else {
        _measuredTemperature = ( workingSample - _b2 ) / _m2;
    }    

    #ifdef DEBUG
    Serial.print(_name);
    Serial.print(",");
    //Serial.print(_VoutAnalogSample);
    //Serial.print(workingSample);
    Serial.print(_measuredTemperature);
    //Serial.print(GetMedianAverage(_rTemperatureMeasurements, 6));
    Serial.print(",");

    if(ln) Serial.println("");
    #endif
  }
}

float Temperature::GetMedian(int array[]){
    int sorted[TEMPERATURE_AVERAGE_VALUE_I];
    float value = 0.0;

    for(int x = 0; x < TEMPERATURE_AVERAGE_VALUE_I; x++) {
        sorted[x] = array[x];
    }

     //ARRANGE VALUES
    for(int x = 0; x < TEMPERATURE_AVERAGE_VALUE_I; x++){
        for(int y = 0; y < TEMPERATURE_AVERAGE_VALUE_I - 1; y++){
            if(sorted[y]>sorted[y+1]){
                int temp = sorted[y+1];
                sorted[y+1] = sorted[y];
                sorted[y] = temp;
            }
        }
    }

    //CALCULATE THE MEDIAN (middle number)
    if(TEMPERATURE_AVERAGE_VALUE_I % 2 != 0){// is the # of elements odd?
        value = (float) sorted[((TEMPERATURE_AVERAGE_VALUE_I-1)/2) + 1];
    }
    else{// then it's even! :)
        value = ((float) ( sorted[(TEMPERATURE_AVERAGE_VALUE_I/2)] + sorted[TEMPERATURE_AVERAGE_VALUE_I/2 + 1] )) / 2.0;
    }


    return value;
}

float Temperature::GetMedianAverage(int array[], int range) {
    int sorted[TEMPERATURE_AVERAGE_VALUE_I];
    float value = 0.0;

    for(int x = 0; x < TEMPERATURE_AVERAGE_VALUE_I; x++) {
        sorted[x] = array[x];
    }

     //ARRANGE VALUES
    for(int x = 0; x < TEMPERATURE_AVERAGE_VALUE_I; x++){
        for(int y = 0; y < TEMPERATURE_AVERAGE_VALUE_I - 1; y++){
            if(sorted[y]>sorted[y+1]){
                int temp = sorted[y+1];
                sorted[y+1] = sorted[y];
                sorted[y] = temp;
            }
        }
    }

    //CALCULATE THE MEDIAN (middle number)
    if(TEMPERATURE_AVERAGE_VALUE_I % 2 != 0){// is the # of elements odd?
        int temp = ((TEMPERATURE_AVERAGE_VALUE_I+1)/2)-1;
        value = (float) sorted[temp];
    }
    else{// then it's even! :)
        value = ((float) ( sorted[(TEMPERATURE_AVERAGE_VALUE_I/2)-1] + sorted[TEMPERATURE_AVERAGE_VALUE_I/2] )) / 2.0;
    }

    int range_ = range;
    if(range_ % 2 != 0) {// is the # of elements odd?
        range_++;
    }

    int sum = 0;
    for( int x = 0; x < range_; x++) {
        sum += sorted[ (TEMPERATURE_AVERAGE_VALUE_I / 2 - range_ / 2 + x ) ];
    }
    value = ((float) (sum / range_));

    return value;
}

float Temperature::GetMode(float new_array[]) {
    int ipRepetition[TEMPERATURE_AVERAGE_VALUE_I];
    for (int i = 0; i < TEMPERATURE_AVERAGE_VALUE_I; i++) {
        ipRepetition[i] = 0;//initialize each element to 0
        int j = 0;//
        while ((j < i) && (new_array[i] != new_array[j])) {
            if (new_array[i] != new_array[j]) {
                j++;
            }
        }
        (ipRepetition[j])++;
    }
    int iMaxRepeat = 0;
    for (int i = 1; i < TEMPERATURE_AVERAGE_VALUE_I; i++) {
        if (ipRepetition[i] > ipRepetition[iMaxRepeat]) {
            iMaxRepeat = i;
        }
    }

    return new_array[iMaxRepeat];
}

float Temperature::getCurrentTemperature() {
    return _measuredTemperature; // - 4.41;
}

float Temperature::setPumpStatus( bool pump ) {
    _pump = pump;

    return _pump;
}
