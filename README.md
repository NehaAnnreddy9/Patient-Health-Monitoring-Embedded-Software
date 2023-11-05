
**Main Idea:
** 
To develop a smart patient monitoring system which helps doctors and other healthcare providers to provide critical and emergency care to patients.
 
**Project Plan:
**
•	Monitor the heart rate of a patient using a pulse sensor. 
•	Monitor body temperature of the patient using a temperature sensor.
•	Monitor any movement of the patient using motion sensor and alert the nurse or doctor.
•	Finally send mail to doctor or healthcare provider in case of abnormal conditions.

**CODE EXPLANATION-
**
•	Thread priorities- Fahrenheit temperature variable is shared variable among Pulse sensor and temperature sensor threads. In this case, pulse sensor is given highest priority because of the sensitive nature of the pulse going up and down and according to that we send alert to health care providers through mail. Motion sensor is given least priority, but it does not access any shared variable.

•	Motion Sensor Explanation – Since the GPIO connected to motion sensor given values 1 when motion detected and 0 when no motion detected, we gave if and else conditions and switched on the LED according to that. 

•	Temperature Sensor Explanation – The analog values read by the sensor are converted to millivolts and from millivolts to Celsius temperature by following the TMP36 sensor datasheet.

•	Pulse Sensor Explanation – Before placing resistors the values read from AIN3 port of Pulse Sensor were very high. After releasing the problem with having high voltage input to beaglebone we added the resistors and the values when no finger placed were a constant 1500. Hence this was taken as the threshold value, if this threshold is exceeded then pulse is detected, and count value is incremented. The value of the sensor increases and then decreases following our pulse, so we placed another condition for less than threshold. One increase and decrease indicate one pulse reading. The RED_LED also follows this increase and decrease of pulse from threshold. If BPM>100 or BPM<60, we are ringing a buzzer for 2 seconds.

HARDWARE CONNECTIONS-

To begin with we connected the beagle bone to our PC with a power cable. Then we made following connections-

●	Temperature Sensor - Output pin of sensor connected to analog input port AIN3 (port P9_38) of BeagleBone and other two pins to VCC (port P9_03) & GND (port P8_01).

●	Pulse Sensor - Output pin of sensor connected to analog input port AIN5 (port P9_36) of BeagleBone and other two pins to VCC (port P9_04) & GND (port P8_01).

●	Motion Sensor - Output pin of sensor connected to digital input port GPIO_69 (port P8_9) of BeagleBone and other two pins to VCC (port P9_05) & GND (port P8_02).

●	Buzzer - The positive pin was connected to GPIO_20 (port P9_41) of BeagleBone and the negative pin is connected to GND (port P8_02).

●	Heartbeat LED - To capture live heartbeat of a patient we used a RED LED. The short pin of LED is connected GND (port P8_02) and the positive pin is connected to GPIO_48 (port P9_15).

●	Motion LED - To capture motion of a patient we used a GREEN LED. The short pin of LED is connected GND (port P8_02) and the positive pin is connected to GPIO_68(port P8_10).

●	We also used 2 resistors (330R and 220R) in series to bring the high voltage output of pulse sensor.

To fit the above connections, we also used two breadboards to make connections.

**TEST PROCESS AND RESULTS-
**
●	Pulse Sensor Functionality Testing - We tested the correct working of our Pulse sensor and code using a Samsung Heartrate Measure(screenshots) and Apple iWatch which detects the pulse rate of the wearer at the same time our pulse sensor. These were the results-

1)	Samsung Heartrate Measure – 91 BPM, Patient Monitoring system - 90 BPM (check time on images – 10:03PM)
                                                                        
2)	Samsung Heartrate Measure – 89 BPM, Patient Monitoring system - 86 BPM (check time on images – 9:53PM) 

3)	Samsung Heartrate Measure – 88 BPM, Patient Monitoring system - 84 BPM (check time on images – 9:23PM)

4)	Apple watch - 80 BPM, Patient Monitoring system - 77 BPM
   
5)	Apple watch - 65 BPM, Patient Monitoring system - 69 BPM
   
The results of our system proved to be close to accurate.

●	Temperature Sensor Functionality Testing - We tested the correct working of our Temperature sensor and code using a Honeywell’s thermostat which detects the temperature of a room at the same time our sensor. These were the results obtained at different times of a day.

1)	Honeywell’s thermostat- 73 F(Fahrenheit), Patient Monitoring system - 72 F (check time on images – 9:25PM)

2)	Honeywell’s thermostat- 73F, Patient Monitoring system - 73F (check time on images – 9:40PM)
 
The results of our system proved to be very accurate.

●	Motion Sensor Functionality Testing - For motion sensor testing we moved our hands multiple times in front of the sensor and checked if that is being detected by accessing the GPIO port value (1 for ‘movement’ and 0 for ‘no movement’). The sensor was highly sensitive to even the slightest movements. One limitation we found for the sensor is that it takes few seconds after switching it ON to get start detecting movements. So, we tested it only few seconds after starting it.  The sensor had a wide detection angle of 180 degrees and so a movement in a corner was also detected.

SENDING DATA-

The following data is sent to doctor or healthcare providers through mail whenever abnormal health conditions are detected in a patient. The below is a screenshot which indicates abnormal health conditions of patients –
 

PREEMPT_RT-

•	We ran our project on PREEMPT_RT. Below screenshot shows our project running on PREEMPT_RT.
 

REFERENCES-
We referred these following links for understanding conversions from analog to temperature and BPM-
•	https://www.electroschematics.com/heart-rate-sensor/#:~:text=Once%20a%20valid%20signal%20is,number%20of%20beats%20per%20minute
•	https://bwgz57.wordpress.com/2012/04/01/beaglebone-how-hot-is-it/

