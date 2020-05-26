# Chicken Egg Incubator

This is the development of chicken egg incubator. The incubator will have temperature and humidity control in ways of heater and fan.
### Temperature and RH measurement
- The temperature should be kept between 37.5 and 39 degC and relative humidity between 60 and 70 %. DHT12 sensor was chosen fortemperature and relative humidity measurements. Communication is via I2C bus.
### Stepper motor control
- Eggs will be turned for 19 days, each hour for one turn. The incubation process should take around 21 days. For precise turning stepper motor control will be implemented. 
### External RTC implementation
- To have data about time, and exact elapsed time external realtime clock module DS3231 is interfaced via I2C bus.
### LCD Display implementation
-User should see the temperature and humidity, and to have information about the time and date, and also about current
incubation process time. For this intention LCD1602 Display was used. Communication is via I2C bus.
### Main MCU choice 
- For this application stm32f407vgt6 MCU was chosen as a part of STM Discovery development board.
### Versioning implemented
- To prevent overwriting the code and accidental loss of code version controlling is implemented also via Git.
### Future development possibilities
- Future development can include adding precise amounts of water via an injection nozzle to achieve precise requested
humidity values. When this is implemented one can make easy corrections to control parameters for other types of eggs, not just
poultry ones. Later option can be added that user can choose which eggs he wants to hatch inside of the incubator. 
