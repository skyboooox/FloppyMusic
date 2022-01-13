#ifndef sensor_h
#define sensor_h

#include "Adafruit_TCS34725.h"
#define COLOR_SENSOR_INTEGRATION TCS34725_INTEGRATIONTIME_614MS
Adafruit_TCS34725 tcs = Adafruit_TCS34725(COLOR_SENSOR_INTEGRATION, TCS34725_GAIN_1X);

int color_state = 0;

uint16_t TCS_r, TCS_g, TCS_b, TCS_c;

void task_color(void *)
{
  static long last_read = millis();
  long integration = ((256 - COLOR_SENSOR_INTEGRATION) * 12 / 5 + 1);
  // uint16_t TCS_r = tcs.read16(TCS34725_RDATAL);
  while (true)
  {
    if (last_read + integration < millis())
    {
      last_read = millis();
      TCS_c = tcs.read16(TCS34725_CDATAL);
      TCS_r = tcs.read16(TCS34725_RDATAL);
      TCS_g = tcs.read16(TCS34725_GDATAL);
      TCS_b = tcs.read16(TCS34725_BDATAL);
      //if (TCS_r > 550 && TCS_c > 2000)
      if (TCS_r > 2500 && TCS_c > 5000)
      {
        color_state = 1;
        // Serial.println("insert disk");
      }
      else
      {
        color_state = 0;
        // Serial.println("disk lost");
      }
    }
    vTaskDelay(10);
  }
}

void setup_sensors()
{
  Wire.begin(8, 9); //颜色传感器I2C
  tcs.begin();
  xTaskCreate(task_color, "task_color", 2048, NULL, 2, NULL);
}

#endif