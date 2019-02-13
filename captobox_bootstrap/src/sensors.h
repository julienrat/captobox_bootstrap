String sensors_labels[] = {"Température", "Humidité", "Pression", "CO2", "COV", "Test", "Non%20utilisé"}; //liste des capteurs
String sensors[] = {"name_capt1", "name_capt2", "name_capt3", "name_capt4"};
const int nb_of_sensors = 6; //nombre de capteurs

String name_capt[nb_of_sensors];
String value_capt[nb_of_sensors];
String value_min_capt[nb_of_sensors];
String value_max_capt[nb_of_sensors];
String value_unit_capt[nb_of_sensors];

void mesure(int num, int num_capt) {
  uint8_t chipID;

  chipID = BME280.readChipId();

  Serial.print("ChipID = 0x");
  Serial.println(chipID, HEX);

  if(chipID==0xFF){
    Serial.println("Erreur BMP280");
  }else{
    BME280.writeMode(smForced);
    BME280.readCompensationParams();
    // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
    BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
    BME280.writeOversamplingTemperature(os1x);
    BME280.writeOversamplingHumidity(os1x);
    // example of a forced sample.  After taking the measurement the chip goes back to sleep
    //BME280.writeMode(smForced);
    Serial.println("mesure ...");
    while (BME280.isMeasuring()) {
      delay(50);
    }
    Serial.println("mesure OK");
    BME280.readMeasurements();
    iaq.readRegisters();
  }
  switch (num) {
    case 0:
    Serial.print("capteur ");
    Serial.println(num_capt);
    Serial.print("mesure de la température : ");
    value_capt[num_capt] = BME280.getTemperature();
    Serial.println(value_capt[num_capt]);
    break;
    case 1:
    Serial.print("capteur ");
    Serial.println(num_capt);
    Serial.print("mesure de l'humidité : ");
    value_capt[num_capt] = BME280.getHumidity();
    Serial.println(value_capt[num_capt]);
    break;
    case 2:
    Serial.print("capteur ");
    Serial.println(num_capt);
    Serial.print("mesure de la pression : ");
    value_capt[num_capt] = BME280.getPressure();
    Serial.println(value_capt[num_capt]);
    break;
    case 3:
    Serial.print("capteur ");
    Serial.println(num_capt);
    Serial.print("mesure du CO2 : ");
    value_capt[num_capt] = iaq.getPrediction();
    Serial.println(value_capt[num_capt]);
    break;
    case 4:
    Serial.print("capteur ");
    Serial.println(num_capt);
    Serial.print("mesure du COV : ");
    value_capt[num_capt] = iaq.getPrediction();
    Serial.println(value_capt[num_capt]);
    break;
    case 5: 
    // Capteur de test
    Serial.print("capteur test : ");
    value_capt[num_capt] = random(0,255);
    Serial.println(value_capt[num_capt]);
    break;
  }
}
