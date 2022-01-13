
#include <WiFi.h>
#include "sensor.h"
#include "floppy.h"
#include "utils.h"
#include "touch.h"
#include "led.h"
#include "player.h"
#include "sheet.h"

void task_debug(void *)
{
  while (true)
  {
    DBG("DISK", color_state);
    // DBG("R", TCS_r);
    // DBG("G", TCS_g);
    // DBG("B", TCS_b);
    // DBG("C", TCS_c);
    // Serial.print(", ");
    DBG("CHN_KEY", chn_key[CHN_TOUCH]);
    DBG("CDELAY", current_delay);
    Serial.println();
    vTaskDelay(100);
  }
}

void task_logic(void *)
{
  while (true)
  {
    vTaskDelay(100);
    if (color_state > 0)
    {
      player_play("newyear", json, 1);
    }
    else
    {
      player_stop(true);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
  setup_floppy();
  setup_sensors();
  setup_touch();
  setup_leds();
  setup_player();
  // xTaskCreate(task_touch, "task_touch", 4096, NULL, 1, NULL);
  xTaskCreate(task_debug, "task_debug", 10240, NULL, 5, NULL);
  xTaskCreate(task_logic, "task_logic", 2048, NULL, 5, NULL);
}

//
void loop()
{
  delay(100);
}
