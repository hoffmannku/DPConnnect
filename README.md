## DPConnnect
Diagnosis tool for reading Vanmoof S3/X3 Battery.  
Reads Vanmoof S3/X3 battery registers via Modbus/RTU. 
The program works with ATmega32u4 based boards e.g. arduino, esp32 
UI is any console e.g. putty
Interface to battery is serial connection(UART)

Wiring Battery with board
- RX       <->  D3 (Softwaresimulated TX)
- TX       <->  D4 (Softwaresimulated RX)
- GND      <->  GND 
- TST      <->  GND  
    
Notes:
1. Onboard RX/TX pin can be used if usb is not connected 
2. Battery managment system must be started. This is done by connecting test pin to GND

This program references an external library [ModbusRTUMAster Library](https://github.com/CMB27/ModbusRTUMaster) 
