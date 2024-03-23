# DPConnnect
## a Tool for reading Vanmoof S3/X3 Battery.  
Reads Vanmoof S3/X3 battery registers via Modbus/RTU. 
The program works with ATmega32u4 based boards e.g. arduino, esp32 
UI is any console e.g. putty
Interface to battery is a serial connection  (UART)

Wiring Battery with Arduino: 
- RX       <->  D3 (Softwaresimulated TX)
- TX       <->  D4 (Softwaresimulated RX)
- GND      <->  GND 
- TST      <->  GND<sup>2</sup>   
    
Notes:
1. most arduinos have only one onboard serial connector, which is used by USB . 
2. battery managment system must be startet. This can be achieved by connection test pin to GND 
