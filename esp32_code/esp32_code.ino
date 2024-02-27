#include <PS4Controller.h>

#define RXD2 16
#define TXD2 17

unsigned long lastTimeStamp = 0;

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

  //Only needed to print the message properly on serial monitor. Else we dont need it.
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

  int x_data = PS4.LStickX();
  int y_data = PS4.LStickY();

  int x_output;
  int y_output;

  if (x_data <= -97) {
    x_output = 1;
  } else if (x_data >= -96 && x_data <= -65) {
    x_output = 2;
  } else if (x_data >= -64 && x_data <= -33) {
    x_output = 3;
  } else if (x_data >= -32 && x_data <= 32) {
    x_output = 4;
  } else if (x_data >= 33 && x_data <= 64) {
    x_output = 5;
  } else if (x_data >= 65 && x_data <= 96) {
    x_output = 6;
  } else if (x_data >= 97) {
    x_output = 7;
  }

  if (y_data <= -97) {
    y_output = 1;
  } else if (y_data >= -96 && y_data <= -65) {
    y_output = 2;
  } else if (y_data >= -64 && y_data <= -33) {
    y_output = 3;
  } else if (y_data >= -32 && y_data <= 32) {
    y_output = 4;
  } else if (y_data >= 33 && y_data <= 64) {
    y_output = 5;
  } else if (y_data >= 65 && y_data <= 96) {
    y_output = 6;
  } else if (y_data >= 97) {
    y_output = 7;
  }

  int combined_output = (x_output << 4) | y_output;
  Serial2.write(combined_output);
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
