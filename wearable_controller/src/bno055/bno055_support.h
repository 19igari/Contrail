#include <Wire.h>
#include "src/bno055/bno055.h"

// ---- BNO055 SensorAPI が要求するコールバック ----
static s8 bno055_i2c_bus_read(u8 dev_addr, u8 reg_addr, u8 *data, u8 len) {
  Wire.beginTransmission(dev_addr);   // dev_addr は 0x28/0x29 の 7-bit アドレス想定
  Wire.write(reg_addr);
  if (Wire.endTransmission(false) != 0) {  // repeated start
    return -1;
  }

  u8 read_len = Wire.requestFrom((int)dev_addr, (int)len);
  if (read_len != len) {
    // 足りない場合でも読み捨てしてバスを健全化
    while (Wire.available()) (void)Wire.read();
    return -1;
  }

  for (u8 i = 0; i < len; i++) {
    data[i] = (u8)Wire.read();
  }
  return 0;
}

static s8 bno055_i2c_bus_write(u8 dev_addr, u8 reg_addr, u8 *data, u8 len) {
  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  for (u8 i = 0; i < len; i++) {
    Wire.write(data[i]);
  }
  if (Wire.endTransmission() != 0) {
    return -1;
  }
  return 0;
}

static void bno055_delay_msec(u32 msec) {
  delay((unsigned long)msec);
}

// ---- BNO055 driver handle ----
static struct bno055_t bno;

// ---- 初期化 ----
static bool bno055_setup() {
  Serial.println("1");
  bno.dev_addr   = BNO055_I2C_ADDR2;
  bno.bus_read   = bno055_i2c_bus_read;
  bno.bus_write  = bno055_i2c_bus_write;
  bno.delay_msec = bno055_delay_msec;

  int res = bno055_init(&bno);
  Serial.println(res);

  if (res != 0) return false;

  // 念のため CONFIG → power normal → ACCONLY（加速度のみ）
  if (bno055_set_operation_mode(BNO055_OPERATION_MODE_CONFIG) != 0) return false;
  delay(25);

  if (bno055_set_power_mode(BNO055_POWER_MODE_NORMAL) != 0) return false;
  delay(10);

  if (bno055_set_operation_mode(BNO055_OPERATION_MODE_ACCONLY) != 0) return false;
  delay(25);

  return true;
}