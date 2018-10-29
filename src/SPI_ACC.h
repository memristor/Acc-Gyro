#ifndef SPI_ACC 
#define SPI_ACC

#include<SPI.h>

#define READ 0
#define WRITE 1
#define ACC_CTRL 0x01
#define ACC_STATUS 0x02
#define ACC_RESET 0x03
#define ACC_X_LSB 0x04
#define ACC_X_MSB 0x05
#define ACC_Y_LSB 0x06
#define ACC_Y_MSB 0x07
#define ACC_Z_LSB 0x08
#define ACC_Z_MSB 0x09
#define ACC_TEMP_LSB 0x12
#define ACC_TEMP_MSB 0x13
#define ACC_INT_STATUS 0x16
#define ACC_ID 0x27
static int accPin = 10;

uint16_t accCmd(uint8_t addr, bool rw, uint8_t data)
{
  int br = 0;
  uint8_t fByte = (addr << 2) | (rw << 1);
  for (int i = 0; i < 8; i++)
  {
    if (fByte & (1 << i))
    {
      ++br;
    }
  }
  fByte |= (br % 2 == 0);

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(accPin, LOW);
  uint8_t rfirst = SPI.transfer(fByte);
  uint8_t rsecond = SPI.transfer(data);
  digitalWrite(accPin, HIGH);
  SPI.endTransaction();

  uint16_t retVal = rsecond | (rfirst << 8);
  return retVal;
}

bool accInit(int p)
{
  accPin = p;
  SPI.begin();
  pinMode(accPin, OUTPUT);
  digitalWrite(accPin, HIGH);
  delay(35);
  uint16_t check = accCmd(ACC_INT_STATUS, READ, 0);
  if (check & 0x40FF)
  {
    return false;
  }
  check = accCmd(ACC_CTRL, WRITE, 0);
  delay(10);
  return true;
}

double accGetTemp()
{
  uint16_t MSB = accCmd(ACC_TEMP_MSB, READ, 0) & 0x00FF;
  uint16_t LSB = accCmd(ACC_TEMP_LSB, READ, 0) & 0x00FF;
  uint32_t val = (MSB<<8) | LSB;
  double retVal = 23+((val/2)-4096)/25.6;
  return retVal;
}

double accGetX()
{
  uint16_t MSB = accCmd(ACC_X_MSB, READ, 0) & 0x00FF;
  uint16_t LSB = accCmd(ACC_X_LSB, READ, 0) & 0x00FF;
  uint32_t val = (MSB<<8) | LSB;
  val=(val>>1)|(val & 0x8000);
  int16_t retVal = val;
  
  return retVal/1800.0*9.81;
}

double accGetY()
{
  uint16_t MSB = accCmd(ACC_Y_MSB, READ, 0) & 0x00FF;
  uint16_t LSB = accCmd(ACC_Y_LSB, READ, 0) & 0x00FF;
  uint32_t val = (MSB<<8) | LSB;
  val=(val>>1)|(val & 0x8000);
  int16_t retVal = val;
  
  return retVal/1800.0*9.81;
}

double accGetZ()
{
  uint16_t MSB = accCmd(ACC_Z_MSB, READ, 0) & 0x00FF;
  uint16_t LSB = accCmd(ACC_Z_LSB, READ, 0) & 0x00FF;
  uint32_t val = (MSB<<8) | LSB;
  val=(val>>1)|(val & 0x8000);
  int16_t retVal = val;
  
  return retVal/1800.0*9.81;
}

#endif
