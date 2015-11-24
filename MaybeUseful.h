/*
void termometerCalibration() {
  float temperaturePumpOFF = 200.0;
  float temperaturePumpON = 200.0;

#ifdef DEBUG
  debugPrintFunction("termometerCalibration");
#endif

  for( int i = 0; i < 300; i++ ) {
    basePT100.measure();
    delay(100);
  }

  temperaturePumpOFF = basePT100.getCurrentTemperature();


  analogWrite(PUMP_PIN, PUMP_SPEED_MAX);

  for( int i = 0; i < 300; i++ ) {
    basePT100.measure();
    delay(100);
  }

  temperaturePumpON = basePT100.getCurrentTemperature();

  analogWrite(PUMP_PIN, PUMP_SPEED_STOP);

#ifdef DEBUG
  debugPrintVar("temperaturePumpOFF", temperaturePumpOFF);
  debugPrintVar("temperaturePumpON", temperaturePumpON);
  debugPrintVar("diff", temperaturePumpON - temperaturePumpOFF);
#endif
}
*/