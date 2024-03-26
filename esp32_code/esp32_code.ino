#include <PS4Controller.h>

#define RXD2 16
#define TXD2 17

unsigned long lastTimeStamp = 0;

int previousVal = 0;

void notify()
{
  char messageString[200];
  sprintf(messageString, "%4d,%4d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d",
          PS4.LStickX(),
          PS4.LStickY(),
          PS4.RStickX(),
          PS4.RStickY(),
          PS4.Left(),
          PS4.Down(),
          PS4.Right(),
          PS4.Up(),
          PS4.Square(),
          PS4.Cross(),
          PS4.Circle(),
          PS4.Triangle(),
          PS4.L1(),
          PS4.R1(),
          PS4.L2(),
          PS4.R2(),
          PS4.Share(),
          PS4.Options(),
          PS4.PSButton(),
          PS4.Touchpad(),
          PS4.Charging(),
          PS4.Audio(),
          PS4.Mic(),
          PS4.Battery());

  // Only needed to print the message properly on serial monitor. Else we dont need it.
  if (millis() - lastTimeStamp > 50)
  {
    Serial.println(messageString);
    lastTimeStamp = millis();
  }

  // struct: 0x(x-axis)(y-axis)
  // 1-3: left/down
  // 4: nothing
  // 5-7: right/up
  // 1/7 fastest mode

  int buttonData = PS4.Circle();
  int xData = PS4.LStickX();
  int yData = PS4.LStickY();

  int xOutput;
  int yOutput;

  if (xData <= -97)
  {
    xOutput = 1;
  }
  else if (xData >= -96 && xData <= -65)
  {
    xOutput = 2;
  }
  else if (xData >= -64 && xData <= -33)
  {
    xOutput = 3;
  }
  else if (xData >= -32 && xData <= 32)
  {
    xOutput = 4;
  }
  else if (xData >= 33 && xData <= 64)
  {
    xOutput = 5;
  }
  else if (xData >= 65 && xData <= 96)
  {
    xOutput = 6;
  }
  else if (xData >= 97)
  {
    xOutput = 7;
  }

  if (yData <= -97)
  {
    yOutput = 1;
  }
  else if (yData >= -96 && yData <= -65)
  {
    yOutput = 2;
  }
  else if (yData >= -64 && yData <= -33)
  {
    yOutput = 3;
  }
  else if (yData >= -32 && yData <= 32)
  {
    yOutput = 4;
  }
  else if (yData >= 33 && yData <= 64)
  {
    yOutput = 5;
  }
  else if (yData >= 65 && yData <= 96)
  {
    yOutput = 6;
  }
  else if (yData >= 97)
  {
    yOutput = 7;
  }

  int combinedOutput = (buttonData << 6) | (xOutput << 3) | yOutput;
  // Serial.print(buttonData);
  // Serial.print(", ");
  // Serial.print(xOutput);
  // Serial.print(", ");
  // Serial.print(yOutput);
  // Serial.print(", ");
  // Serial.println(combinedOutput);
  if (previousVal != combinedOutput)
  {
    previousVal = combinedOutput;
    Serial.print(combinedOutput);
    Serial2.write(combinedOutput);
  }
}

void onConnect()
{
  Serial.println("Connected!.");
}

void onDisConnect()
{
  Serial.println("Disconnected!.");
}

void setup()
{
  Serial.begin(115200);
  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisConnect);
  PS4.begin();
  Serial.println("Ready.");

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop()
{
}
