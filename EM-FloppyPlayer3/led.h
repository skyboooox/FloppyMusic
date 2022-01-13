#ifndef led_h
#define led_h
#include "floppy.h"
#include "sensor.h"
#define LEDS 6
#define LED(i) 20 - i

int led_mode = 0;
float led_cache[] = {0, 0, 0, 0, 0, 0};
float led_target[] = {0, 0, 0, 0, 0, 0};
float led_ease[] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};

void led_mode_sine()
{
  float time = (float)millis() / 1000;
  for (int i = 0; i < LEDS; i++)
  {
    led_target[i] = sin(time * 3.0 + (float)i / 4.0f) * 0.5 + 0.5;
    led_ease[i] = 0.2;
  }
}

void led_mode_key()
{
  static int disk_insert = 0;
  float time = (float)millis() / 1000;
  for (int i = 0; i < LEDS; i++)
  {
    if (disk_insert == 0 && color_state > 0)
    {
      led_target[i] = 1;
      led_ease[i] = 1;
      continue;
    }
    if (current_key == 0 || (current_key % 6 != i))
    {
      led_target[i] = 0;
      led_ease[i] = 0.02;
    }
    else
    {
      led_target[i] = sin(time * 20.0) * 0.3 + 0.7;
      led_ease[i] = 0.3;
    }
  }
  disk_insert = color_state;
}

void task_leds(void *)
{
  while (true)
  {
    led_mode_key();
    for (int i = 0; i < LEDS; i++)
    {
      led_cache[i] += (led_target[i] - led_cache[i]) * led_ease[i];
      // changing the LED brightness with PWM
      ledcWrite(i, (int)(led_cache[i] * 255));
    }
    vTaskDelay(5);
  }
}

void setup_leds()
{
  for (int i = 0; i < LEDS; i++)
  {
    ledcSetup(i, 5000, 8);
    ledcAttachPin(LED(i), i);
    // pinMode(LED(i), OUTPUT);
    // digitalWrite(LED(i), HIGH);
  }
  xTaskCreate(task_leds, "task_leds", 2048, NULL, 3, NULL);
}

#endif