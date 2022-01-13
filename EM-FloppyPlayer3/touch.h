#ifndef touch_h
#define touch_h

#include "utils.h"

void task_touch(void *)
{
  chn_flag[CHN_TOUCH] = 1;
  static int all[] = {
      1, 10, 2, 11, 3, 4, 12, 5, 13, 6, 14, 7};

  static int delays[] = {
      130, 138, 147, 155, 164, 174, 185, 196, 207, 220, 233, 246,
      261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493};

  static int touchsignal[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  static long last_change = 0;
  static long index = 0;
  while (true)
  {
    int empty = 1;
    for (int i = 0; i < 12; i++)
    {
      touchsignal[i] = touchRead(all[i]);
      yield();
    }

    int max = 0;
    int max_key = -1;
    int min = 100000;
    long sum = 0;

    for (int i = 0; i < 12; i++)
    {
      if (max < touchsignal[i])
      {
        max_key = i;
        max = touchsignal[i];
      }
      if (min > touchsignal[i])
      {
        min = touchsignal[i];
      }
      sum += touchsignal[i];
    }
    // Serial.print("max = ");
    // Serial.print(max);
    // Serial.print(", min = ");
    // Serial.print(min);
    // Serial.print(", sum = ");
    // Serial.println(sum);
    if (max > 600)
    {
      if (min > 210)
      {
        max_key += 12;
      }
      chn_key[CHN_TOUCH] = max_key + 48;
    }
    else
    {
      chn_key[CHN_TOUCH] = 0;
    }
    vTaskDelay(1);
  }
}

void setup_touch()
{
  touchSetCycles(0x0005, 0x0005); //触摸读取频率
  xTaskCreate(task_touch, "task_touch", 4096, NULL, 1, NULL);
}

/*------------------------颜色传感器-----------------------------*/

#endif