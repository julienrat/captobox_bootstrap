# Captobox Bootstrap Version
New version of captobox

Git clone this directory and use in Platformio

## Adding sensor
### Create definitions in sensor.h
- Add your sensor name in line n°1

``` String sensors_labels[] = {"Température", "Humidité", "Pression", "CO2", "COV", "Test","YOUR SENSOR", "Non%20utilisé"}; //liste des capteurs ```

- Count number of sensors and change line n°3

``` const int nb_of_sensors = 6; //nombre de capteurs ``` 
