/*  Read/write BMS in Vanmoof S3 Battery (Dynapack) via Modbus RTU
Pin Connections Battery -> Arduino  
GND -> GND, 
TX-> D3 (=SoftSerial RX), 
RX ->D4 (=SoftSerial TX). 

Version 0.3
*/
#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>
//#include <EEPROM.h>
const char major_version = '0';
const char minor_version = '3';
const uint8_t rxPin = 3;
const uint8_t txPin = 4;
const uint8_t devId = 170;  				//  ID Battery

const uint32_t baudRates[8] = { 1200, 2400, 4800, 9600, 16200, 38400, 57600, 115200 };
const uint8_t  hostBaudRate = 5;        // works ok
const uint8_t  BTBaudRate = 3;          // slow for softwareserial 

const uint8_t holding_regs_read = 100;

enum {
  // register guess
  BT_TYPE = 0,                 // Type
  UNKNOWN_1 = 1,               // ?
  BT_ERR = 2,                  // Error State  0 -> ok
  BT_TEMP = 3,                 // Temp Batterie
  BT_VOLTAGE = 4,              // Voltage
  BT_SOC = 5,                  // State of charge
  BT_CURRENT = 6,              // current (while decharging?)
  UNKNOWN_2 = 7,               // ?
  IS_DISCHARGING_ALLOWED = 8,  //
  IS_TESTMODE_ACTIVE = 9,      //
  BT_HW_VERSION = 10,          //
  BT_SW_VERSION = 11,          //
  BT_SN_FROM = 12,             // Serialnumber 6 words
  BT_MANFT_DATE = 19,          // Manufacturing Date 2 words
  BT_CAP_NOMINAL = 21,         // 21: Capacity in mAh
  BT_CAP_FULL = 22,            //
  BT_CAP_REMAIN = 23,          //
  BT_HEALTH = 24,              //
  BT_CYCLE_COUNT = 25,         //
  UNKNOWN_3 = 26,              // ?
  CE_VOLTAGE_FROM = 27,        // Cell Voltage 1..10 in mV 10 Words
};

SoftwareSerial mySerial(rxPin, txPin);  // arduino nano has no second rx/tx pin pair
ModbusRTUMaster modbus(mySerial);       // no DE_PIN

// user input from serial console
long requestUserInput() {
  Serial.print("> ");
  while (!Serial.available()) {}
  String userInput = Serial.readStringUntil('\n');
  userInput.trim();
  Serial.println(userInput);
  return userInput.toInt();
}

long selectValue(long minValue, long maxValue) {
  long value;
  while (1) {
    value = requestUserInput();
    if (value < minValue or value > maxValue) Serial.println(F("Invalid entry, try again"));
    else return value;
  }
}

void processError() {
  if (modbus.getTimeoutFlag()) {
    Serial.println(F("Connection timed out"));
    modbus.clearTimeoutFlag();
  } else if (modbus.getExceptionResponse() != 0) {
    Serial.print(F("Received exception response: "));
    Serial.print(modbus.getExceptionResponse());
    switch (modbus.getExceptionResponse()) {
      case 1:
        Serial.println(F(" - Illegal function"));
        break;
      case 2:
        Serial.println(F(" - Illegal data address"));
        break;
      case 3:
        Serial.println(F(" - Illegal data value"));
        break;
      case 4:
        Serial.println(F(" - Server device failure"));
        break;
      default:
        Serial.println();
        break;
    }
    modbus.clearExceptionResponse();
  } else {
    Serial.println("An error occurred");
  }
}

//------------------------------------------
// FC 3: Read Analog Output Holding Registers
//------------------------------------------
void readHoldingRegs() {
  uint16_t holdingRegisters[holding_regs_read];  // function 3 register array
  char txt[80];
  float f;
  uint16_t z;

  if (modbus.readHoldingRegisters(devId, 0, holdingRegisters, holding_regs_read)) {
    Serial.print(F("Holding Registers on Device "));
    Serial.println(devId);

    for (uint8_t i = 0; i < holding_regs_read; i++) {
      //sprintf(txt, "reg %d val %X int %u char %c%c", i, holdingRegisters[i], holdingRegisters[i], highByte(holdingRegisters[i]), lowByte(holdingRegisters[i]));
      //Serial.println(txt);
      switch (i) {
        case BT_TYPE:
          sprintf(txt, "Battery type       %X", holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_ERR:
          sprintf(txt, "Errorstate         %X", holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_TEMP:
          z = (int)holdingRegisters[i];
          Serial.print("Battery Temp       ");
          Serial.print((float)z / 100, 2);
          Serial.println(" C");
          break;
        case BT_SOC:
          sprintf(txt, "SOC relative       %u %%", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_VOLTAGE:
          z = (int)holdingRegisters[i];
          Serial.print("Battery Voltage    ");
          Serial.print((float)z / 1000, 2);
          Serial.println(" V");
          break;
        case BT_CURRENT:  // Strom
          sprintf(txt, "Current            %u mA", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case IS_DISCHARGING_ALLOWED:
          sprintf(txt, "Discharging Flag   %u", (bool)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_SN_FROM:  // Serialnumber 7 words
          char snr[8];
          for (uint8_t j = 0; j < 7; j++) {
            snr[j] = lowByte(holdingRegisters[i + j]);
          }
          snr[7] = '\0';
          sprintf(txt, "Serial Number      %s", snr);
          Serial.println(txt);
          break;
        case BT_MANFT_DATE:  // Manufacturing Date 2 words
          sprintf(txt, "Manufact Record    %u%u", holdingRegisters[i], holdingRegisters[i + 1]);
          Serial.println(txt);
          break;
        case BT_CAP_NOMINAL:  // 21: Capacity in mAh
          sprintf(txt, "Capacity nominal   %u mAh", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_CAP_FULL:  //
          sprintf(txt, "Capacity full      %u mAh", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_CAP_REMAIN:  //
          sprintf(txt, "Capacity remaining %u mAh", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_HEALTH:  // ?
          sprintf(txt, "Battery Health     %u ", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case BT_CYCLE_COUNT:  //
          sprintf(txt, "Cycle Count        %u", (int)holdingRegisters[i]);
          Serial.println(txt);
          break;
        case CE_VOLTAGE_FROM:  // Cell Voltage 1..10 in mV 10 Words
          for (uint8_t j = 0; j < 10; j++) {
            sprintf(txt, "Cell Voltage %2u    ", j + 1);
            Serial.print(txt);
            z = (int)holdingRegisters[i + j];
            Serial.print((float)z / 1000, 3);
            Serial.println(F(" V"));
          }
          break;
        case UNKNOWN_1:
        case UNKNOWN_2:
        case UNKNOWN_3:
          char c[2] = { highByte(holdingRegisters[i]), lowByte(holdingRegisters[i]) };
          if (!isascii(c[0])) c[0] = ".";
          if (!isascii(c[1])) c[1] = ".";
          sprintf(txt, "Register %u         %X %c%c", i, holdingRegisters[i], c[0], c[1]);
          Serial.println(txt);
          break;
        default:
          sprintf(txt, "reg %d val %X int %u char %c%c", i, holdingRegisters[i], holdingRegisters[i], highByte(holdingRegisters[i]), lowByte(holdingRegisters[i]));
          Serial.println(txt);
          break;
      }
    }
  } else processError();
}
void setup() {
  Serial.begin(baudRates[hostBaudRate]); // Host baud rate, config to do
  Serial.setTimeout(1000);
  char txt[40];
  while (!Serial) {}
  sprintf(txt, "VM DP Battery Tool %c.%c", major_version, minor_version);
  Serial.println(txt);

  modbus.begin(baudRates[BTBaudRate]);  // modbus baud rate, config to do
  modbus.setTimeout(100);
  Serial.print(F("Modbus serial port configuration: "));
  Serial.print(baudRates[BTBaudRate]);
  Serial.print(F("-8-N-1"));
  Serial.println();
}

void loop() {
  Serial.println(F("Select an action"));
  Serial.println(F("1 - Read Holding Registers"));
  // Serial.println(F("2 - Read output coil value (FC 1)"));
  // Serial.println(F("3 - Read input contact (FC 2) "));
  // Serial.println(F("4 - Read input registers (FC 4) "));
  //  Serial.println(F("3 - Write value"));
  uint16_t address;

  switch (selectValue(1, 4)) {
    case 1:
      readHoldingRegs();
      break;
    default:
      Serial.println(F("Default...."));
      break;
  }
  Serial.println();
}
