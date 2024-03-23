# DPConnnect
Tool for Vanmoof S3 Battery.  
Reads Vanmoof S3/X3 battery registers via Modbus/RTU.
Code is intended for any Arduino Controler. 
UI is any console e.g. putty
interface to battery is a serial connection (RS-485)

Wiring 
Battery        Arduino 
-------       ---------
RX       <->  D3 (Softwaresimulated TX)
TX       <->  D4 (Softwaresimulated RX)
GND      <->  GND 
[TST]    <->  GND   
    
Notes:
(1) most arduinos have only onboard one serial connector pair, which is needed for USB connection to host. 
(2) battery managment system must be startet. This can be achieved by connection test pin to GND 

