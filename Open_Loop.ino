#include <Wire.h> // Arduino library for I2C
#include <math.h>

#define PUMP_STEP PB8

float get_flow_raw();
void pumpStepSignal();
unsigned long flowToDelay(int spd);

int ret;
bool stop = false;

uint16_t sensor_flow_value;
int16_t signed_flow_value;
float scaled_flow_value;
int sensor_flow_crc;

unsigned long last_read = 0;

const int ADDRESS = 0x08; // Sensor I2C Address
const float SCALE_FACTOR_FLOW = 500.0; // Scale Factor for flow rate measurement
const float SCALE_FACTOR_TEMP = 200.0; // Scale Factor for temperature measurement
const char *UNIT_FLOW = " ul/min"; //physical unit of the flow rate measurement
const char *UNIT_TEMP = " deg C"; //physical unit of the temperature measurement

float frekuensi = 1000.0;
float read_time;
float first_read_time;
unsigned int read_count = 0;

TwoWire Wire2 (2,0,100000);

void setup() {
  Serial.begin(115200); // initialize serial communication
  Wire2.begin();       // join i2c bus (address optional for master)
  pinMode(PA12, INPUT_PULLUP);
  pinMode(PUMP_STEP, OUTPUT); // STEP input

  //inisialisasi timer untuk sinyal step pompa
  Timer1.pause();
  Timer1.attachInterrupt(TIMER_CH1, pumpStepSignal);
  Timer1.setPeriod(1000000 / (2 * (unsigned long)frekuensi));
  Timer1.refresh();
  Timer1.resume();

  // Soft reset the sensor
  do {
    Wire2.beginTransmission(0x00);
    Wire2.write(0x06);
    ret = Wire2.endTransmission();
    if (ret != 0) {
      Serial.println("Error while sending soft reset command, retrying...");
      delay(500); // wait long enough for chip reset to complete
    }
  } while (ret != 0);
  delay(30); // wait long enough for chip reset to complete

  // To perform a measurement, first send 0x3608 to switch to continuous
  // measurement mode (H20 calibration), then read 3x (2 bytes + 1 CRC byte) from the sensor.
  // To perform a IPA based measurement, send 0x3615 instead.
  // Check datasheet for available measurement commands.
  do {
    Wire2.beginTransmission(ADDRESS);
    Wire2.write(0x36);
    Wire2.write(0x08);
    ret = Wire2.endTransmission();
    if (ret != 0) {
      Serial.println("Error during write measurement mode command");
    }
  } while (ret != 0);
  delay(12); // wait until measurement is available

  Serial.println("Time (s),Flow Rate (uL/min),Frequency (Hz), Read Out Frequency (Hz)");
}

// -----------------------------------------------------------------------------
// The Arduino loop routine runs over and over again forever:
// -----------------------------------------------------------------------------
void loop() {
  while (!stop) {
    Serial.print(millis()/1000.0);
    Serial.print(",");
    Serial.print(get_flow_raw());
    Serial.print(",");
    Serial.println(frekuensi);
    
    if (digitalRead(PA12) == LOW) {
      stop = true;
    }
  }

  // To stop the continuous measurement, first send 0x3FF9.
  if (stop) {
    Wire2.beginTransmission(ADDRESS);
    Wire2.write(0x3F);
    Wire2.write(0xF9);
    ret = Wire2.endTransmission();
    if (ret != 0) {
      Serial.println("Error during write measurement mode command");
    }

    Timer1.pause(); //menghentikan timer, sehingga pompa berhenti
    delay(999999);
  }
}

float get_flow_raw() {  
  Wire2.requestFrom(ADDRESS, 3);
  if (Wire2.available() < 3) {
    return 0.0;
  }
  sensor_flow_value  = Wire2.read() << 8; // read the MSB from the sensor
  sensor_flow_value |= Wire2.read();      // read the LSB from the sensor
  sensor_flow_crc    = Wire2.read();

  signed_flow_value = (int16_t) sensor_flow_value;
  scaled_flow_value = ((float) signed_flow_value) / SCALE_FACTOR_FLOW * 1000.0;

  return scaled_flow_value;
}

// Fungsi yang mengatur sinyal square untuk menjalankan pompa
void pumpStepSignal() {
  static int signal_state = 0;
  if (signal_state == 0) {
    digitalWrite(PUMP_STEP, HIGH);
    signal_state = 1;
  }
  else {
    digitalWrite(PUMP_STEP, LOW);
    signal_state = 0;
  }
}