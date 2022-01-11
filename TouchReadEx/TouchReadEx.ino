void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  touchSetCycles(0x0001, 0x0001);
}

void loop()
{
  static int all[] = {
      1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14};
  for (int i = 0; i < 13; i++)
  {
    int value = touchRead(all[i]);
    if (value > 300)
    {
      Serial.print("touch ");
      Serial.print(all[i]);
      Serial.print(" = ");
      Serial.println(value);
    }
  }
  delay(5);
}
