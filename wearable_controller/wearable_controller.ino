#include <Wire.h>
#include "src/bno055/bno055_support.h"

void setup() {
  Serial.begin(115200);
  delay(200);
  while (!Serial) {}

  Wire.begin();              // SDA/SCL はボード依存
  Wire.setClock(400000);     // BNO055はFast-mode(400k)対応

  if (!bno055_setup()) {
    Serial.println("BNO055 init failed.");
    while (1) delay(1000);
  }
  Serial.println("BNO055 ready.");
}

void loop() {
  struct bno055_accel_t acc; // x,y,z は s16

  if (bno055_read_accel_xyz(&acc) == 0) {
    // Bosch APIのACC_DATAは通常 1 LSB = 1 mg
    const float ax_g = acc.x / 1000.0f;
    const float ay_g = acc.y / 1000.0f;
    const float az_g = acc.z / 1000.0f;

    Serial.print(" | [g] x=");
    Serial.print(ax_g, 3);
    Serial.print(" y=");
    Serial.print(ay_g, 3);
    Serial.print(" z=");
    Serial.println(az_g, 3);
  } else {
    Serial.println("read accel failed.");
  }

  delay(50);
}
