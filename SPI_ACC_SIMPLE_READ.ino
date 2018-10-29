#include"SPI_ACC.h"
bool initOk = false;
void setup()
{
  Serial.begin(115200);

  initOk = accInit(10);
  if (initOk)
    Serial.println("Acc init ok!");
  else
    Serial.println("Acc init failed!");

}

void loop()
{
  if (initOk)
  {
    Serial.print("X: ");
    Serial.print(accGetX());
    Serial.print(" Y: ");
    Serial.print(accGetY());
    Serial.print(" Z: ");
    Serial.println(accGetZ());
  }
}
