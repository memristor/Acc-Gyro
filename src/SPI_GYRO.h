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

#define BIT_S_OK (1 << 1)
#define BIT_SOFTRESET (1 << 1)
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

  /*Serial.println("Prva i druga rec u gyroCmd:");
  Serial.println(fWord, HEX); //for debugging
  Serial.println(data, HEX);  //for debugging*/

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(gyroPin, LOW);
  
  rfirst = SPI.transfer((fWord & 0xFF00) >> 8) & 0x00FF;
  rfirst = rfirst << 8;
  rfirst |= SPI.transfer(fWord & 0x00FF) & 0x00FF;  //first 16 bits transferred
  
  rsecond = SPI.transfer((data & 0xFF00) >> 8) & 0x00FF;
  rsecond = rsecond << 8;
  rsecond |= SPI.transfer(data & 0x00FF) & 0x00FF;  //second (last) 16 bits transferred
  
  
  /*rfirst = SPI.transfer16(fWord);
    Serial.println(rfirst, HEX); //debugging use
    rsecond = SPI.transfer16(data);*/ 
  //^^^^^this can be used to transfer 16 bits at once instead of splitting it in 1 byte chunks, doesn't affect the problem we have with init

  
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
  uint16_t check = gyroCmd(GYRO_STATUS_CONFIG, READ, 0);

  if (check & BIT_S_OK)
    return true;
  else
  {
    gyroCmd(GYRO_IC_ID, WRITE, BIT_SOFTRESET);  //do a software reset
    delay(800);
    
    gyroCmd(GYRO_STATUS_CONFIG, READ, 0);
    check = gyroCmd(GYRO_STATUS_CONFIG, READ, 0); //read status register twice to clear error flags
    
    Serial.print("Status register after soft reset(hex): ");  //for debugging
    Serial.println(check, HEX);
    
    return false;
  }
}

double gyroGetX()
{
  //has to be tested, solve init problem first
  uint16_t val = gyroCmd(GYRO_RATE_X, READ, 0);
  val = (val >> 2) | (val & 0x8000) | 0x4000;

  int16_t retVal = val;
  return retVal / 50.0;
}

double gyroGetTemp()
{
  //has to be tested, solve init problem first
  uint16_t val = gyroCmd(GYRO_TEMP, READ, 0);
  Serial.println(val, HEX);
  val = (val >> 2) | (val & 0x8000);// | 0x4000;
  Serial.println(val, HEX);

  int16_t retVal = val;
  return (retVal + 3250) / 65.0;

}
#endif
