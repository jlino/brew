/*
  Temperature.h - Library for measuring temperature with a Temperature.
  Created by João Lino, June 24, 2015.
  Released into the public domain.
*/
#ifndef Temperature_h
#define Temperature_h

#include "Arduino.h"

#define DEBUG

#define CONSTANT_ADC_STEP_COUNT 	1024.0

#define TEMPERATURE_AVERAGE_VALUE_I				50
#define TEMPERATURE_AVERAGE_VALUE_F 			50.0
//#define TEMPERATURE_AVERATE_INIT_VALUES		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
#define TEMPERATURE_AVERATE_INIT_VALUES		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
#define TEMPERATURE_AVERATE_INIT_VALUES_I		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#define TEMPERATURE_SETTING_MAX_VALUE 		120

class Temperature
{
  public:
		// Temperature(Temperature_OUTPUT_PIN, Temperature_INPUT_PIN, Temperature_TIME_BETWEEN_READINGS, Temperature_DEFAULT_ADC_VMAX, Temperature_DEFAULT_VS, Temperature_DEFAULT_R1_RESISTENCE, Temperature_DEFAULT_LINE_RESISTENCE, Temperature_DEFAULT_OPERATION_RESISTENCE);
    Temperature(const char *name,
    				int OutputPin_SensorPower, 
					int InputPin_TemperatureReading, 
					int TimeBetweenReadings, 
                    float m1, 
                    float m2,
                    float b1,
                    float b2);

		void measure(boolean ln);
		void safeHardwarePowerOff();
		
		float getCurrentTemperature();

		float setPumpStatus( bool pump );
  
	private:
		const char	*_name;
		int 			  _OutputPin_SensorPower; 
		int 			  _InputPin_TemperatureReading;
		int 			  _TimeBetweenReadings;
		float 			_m1;
		float 			_m2;
		float 			_b1;
		float 			_b2;
		
		float       _temperatureAverage;
		float       _measuredTemperature;
		float			  _measuredTemperatureDeviation;
		bool			  _pump;
		unsigned long   _lastTemperatureRead;
		int           	_VoutAnalogSample;
		int           	_VoutRAnalogSample;
		float           _VoutPreviousAnalogSample;
		int             _temperatureMeasurementsMarker;
		int             _rTemperatureMeasurementsMarker;
		float           _temperatureMeasurements[TEMPERATURE_AVERAGE_VALUE_I] = {TEMPERATURE_AVERATE_INIT_VALUES};
		int           	_rTemperatureMeasurements[TEMPERATURE_AVERAGE_VALUE_I] = {TEMPERATURE_AVERATE_INIT_VALUES_I};

		float GetMedian(int array[]);
		float GetMedianAverage(int array[], int range);
		float GetMode(float array[]);
};

#endif
