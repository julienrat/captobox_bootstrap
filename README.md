# Captobox Bootstrap Version
New version of captobox

Git clone this directory and use in Platformio
## Installation
Copy paste this repo in your platformio project directory
- upload program
- upload data to Spiffs

## Adding sensor
### Create definitions in sensor.h
- Add your sensor name in line n°1

``` String sensors_labels[] = {"Température", "Humidité", "Pression", "CO2", "COV", "Test","YOUR SENSOR", "Non%20utilisé"}; //liste des capteurs ```

- Count number of sensors and change line n°3

``` const int nb_of_sensors = 6; //nombre de capteurs ``` 

- Add a case and put your code inside

 ```
 case 5: 
    // Capteur de test
    Serial.print("capteur test : ");
    value_capt[num_capt] = random(0,255);
    Serial.println(value_capt[num_capt]);
    break;
```

### Ajust data/config.html file
- Add an entry in sensor selection line n°75, n°112, n°149 and n°185 

```
<select class="form-control" id="sel2"  onchange="autofill_capt2(this.value)">
                  <option>Non utilisé</option> //0
                  <option>Humidité</option> //1
                  <option>Température</option> //2
                  <option>Pression</option> //3
                  <option>CO2</option> //4
                  <option>COV</option> //5
                  <option>Test</option> //6
                  <option>YOUR SENSOR</option> //7 HERE at the end
                </select>
```
