//Arduino library for Gyro SCC1300D02 by Dusan Jovanovic and Milan Savic
//Not tested due to hardware problems
#ifndef SPI_GYRO
#define SPI_GYRO

#include <SPI.h>

#define READ 0
#define WRITE 1

#define GYRO_RATE_X 0x00
#define GYRO_IC_ID 0x07
#define GYRO_STATUS_CONFIG 0x08
#define GYRO_TEMP 0x0A
static int gyroPin = 9;

uint16_t gyroCmd(uint8_t addr, bool rw, uint16_t data)
{
  int br = 0;

  uint16_t fWord = 0;
  fWord = addr;
  fWord = (fWord << 3);
  fWord |= rw << 2;

  for (int i = 0; i < 16; i++)
  {
    if (fWord & (1 << i))
    {
      ++br;
    }
  }
  fWord |= (br % 2 == 0);


  data = data << 1;

  br = 0;
  for (int i = 0; i < 16; i++)
  {
    if (data & (1 << i))
    {
      ++br;
    }
  }
  data |= (br % 2 == 0);

  Serial.println("Prva i druga rec u gyroCmd:");
  Serial.println(fWord, HEX); //for debugging
  Serial.println(data, HEX);  //for debugging

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(gyroPin, LOW);
  uint16_t rfirst = SPI.transfer(fWord);
  Serial.print("Status: "); //for debugging
  Serial.println(rfirst, HEX);  //for debugging
  uint16_t rsecond = SPI.transfer(data);
  digitalWrite(gyroPin, HIGH);
  SPI.endTransaction();

  return rsecond;

}

bool gyroInit(int p)
{
  gyroPin = p;
  SPI.begin();
  pinMode(gyroPin, OUTPUT);
  digitalWrite(gyroPin, HIGH);
  delay(800);

  gyroCmd(GYRO_STATUS_CONFIG, READ, 0);
  bool check = gyroCmd(GYRO_STATUS_CONFIG, READ, 0);

  if (check & 0x3FFE)
    return true;
  else
    return false;
}

double gyroGetX()
{
  uint16_t val = gyroCmd(GYRO_RATE_X, READ, 0);
  val = (val >> 2) | (val & 0x8000) | 0x4000;

  int16_t retVal = val;
  return retVal / 50.0;
}

double gyroGetTemp()
{
  uint16_t val = gyroCmd(GYRO_TEMP, READ, 0);
  Serial.println(val, HEX);
  val = (val >> 2) | (val & 0x8000) | 0x4000;
  Serial.println(val, HEX);

  int16_t retVal = val;
  return (retVal + 3250) / 65.0;

}
#endif
