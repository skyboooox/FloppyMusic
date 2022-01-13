
#include <WiFi.h>

#include "Adafruit_TCS34725.h"
// #define TCS34725_RDATAL (0x16)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

#include <ArduinoJson.h>

/*     FLAGS    */
int has_disk = 0;  //0:无软盘  1:有软盘
long playHead = 0; //播放位置
int web_play = 0;  //0:web停止播放 1:web正在播放

int DIR_PIN = 33;  //软驱方向
int STEP_PIN = 34; //软驱电机步进

int midi_hz;
long touch_hz;
int web_hz;
long current_hz = 0;

const uint16_t note_freq[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                     /*   0 -  11 */
    16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31,                         /*  12 -  23 */
    33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,                         /*  24 -  35 */
    65, 69, 73, 78, 82, 87, 93, 98, 104, 110, 117, 123,                     /*  36 -  47 */
    131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,             /*  48 -  59 */
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,             /*  60 -  71 */
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,             /*  72 -  83 */
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, /*  84 -  95 */
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, /*  96 - 107 */
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, /* 108 - 119 */
    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544                      /* 120 - 127 */
};

char json[] = "{\"sheet\":[1,1,48,1000,2,48,1000,1,50,2000,2,50,2000,1,50,3000,2,50,3000,1,52,4000,2,52,4000,1,54,5000,2,54]}";
//[序号,命令,音符]

/*     占坑管理器     */
void task_queue(void *)
{
  while (true)
  {
    Serial.println(has_disk);

    if (has_disk == 1)
    {

      current_hz = midi_hz;
    }
    if (has_disk == 0)
    {
      current_hz = touch_hz;
    }
  }
}

/*     软驱 电机驱动    */
float hz = 100;
float hz_dir = 0.001;
int prevHz = 0;
long hz_to_us(long hz)
{
  return 2000000 * 1000 / hz;
}
long last_move = 0;
int current_voltage = 0;
int steps = 100;
int dir = 1;

void move()
{
  steps++;
  if (steps > 80)
  {
    dir = dir == 1 ? 0 : 1;
    steps = 0;
    digitalWrite(STEP_PIN, dir);
  }
  // current_voltage = current_voltage == 1 ? 0 : 1;
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
}

float prev_hz = 0;
void step()
{
  if (current_hz == 0)
  {
    prev_hz = 0;
    return;
  }
  if (prev_hz == 0)
  {
    prev_hz = current_hz;
  }
  else
  {
    prev_hz += ((float)current_hz - prev_hz) * 0.0003;
  }
  // long d = hz_to_us(current_hz);
  long d = hz_to_us((long)(prev_hz * 1000));
  if (d + last_move < micros())
  {
    last_move = micros();
    move();
  }
}

void task_step(void *)
{
  while (true)
  {
    step();
    yield();
  }
}

/*     MIDI播放器    */

void task_play(void *)
{
  //load song
  static DynamicJsonDocument doc(40960);
  deserializeJson(doc, json);

  static long first_timer, now_time;
  long midi_time = 1, midi_active, midi_tone;

  JsonArray array = doc["sheet"];

  int amount_number = array.size();

  while (true)
  {

    if (has_disk == 0) //无盘不播放
    {
      vTaskDelay(1);
      continue;
    }
    if (playHead >= amount_number)
    {
      playHead = -1;
      vTaskDelay(1000);
      continue;
    }

    now_time = millis();

    if ((now_time - first_timer) > midi_time && has_disk == 1 && playHead > 0)
    {
      midi_time = doc["sheet"][playHead];
      midi_active = doc["sheet"][playHead + 1];
      midi_tone = doc["sheet"][playHead + 2];

      if (midi_active == 2)
      {
        midi_hz = 0;
      }
      if (midi_active == 1)
      {
        midi_hz = note_freq[midi_tone];
      }
      playHead = playHead + 3;
      midi_time = doc["sheet"][playHead];
      // Serial.println(playHead);
    }
    vTaskDelay(1);

    // Serial.println(now_time);
  }
}

/*     触摸按键    */

void task_melody(void *)
{
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
    if (max > 600)
    {
      if (min > 210)
      {
        max_key += 12;
      }
      touch_hz = (int)delays[max_key];
    }
    else
    {
      touch_hz = 0;
    }
  }
}

/*     颜色传感器    */

void task_color(void *)
{
  // tcs.begin();

  // uint16_t TCS_r = tcs.read16(TCS34725_RDATAL);
  while (true)
  {
    uint16_t TCS_r, TCS_g, TCS_b, TCS_c;

    tcs.getRawData(&TCS_r, &TCS_g, &TCS_b, &TCS_c);
    // TCS_colorTemp = tcs.calculateColorTemperature_dn40(TCS_r, TCS_g, TCS_b, TCS_c);
    // lux = tcs.calculateLux(TCS_r, TCS_g, TCS_b);
    // Serial.print("R: ");
    // Serial.print(TCS_r, DEC);
    // Serial.print(" ");
    // Serial.print("G: ");
    // Serial.print(TCS_g, DEC);
    // Serial.print(" ");
    // Serial.print("B: ");
    // Serial.print(TCS_b, DEC);
    // Serial.print(" ");
    // Serial.print("C: ");
    // Serial.print(TCS_c, DEC);
    // Serial.print(" ");
    // Serial.println(" ");
    // if (TCS_r > 1200 && TCS_g > 1500 && TCS_b > 1500 && TCS_c > 4000)
    if (TCS_r > 500)
    {

      has_disk = 1;
    }
    else
    {
      has_disk = 0;
    }
    vTaskDelay(100);
  }
}

void loop()
{
  delay(100);
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  touchSetCycles(0x0005, 0x0005); //触摸读取频率

  Wire.begin(8, 9); //颜色传感器I2C

  pinMode(DIR_PIN, OUTPUT);  //软驱方向
  pinMode(STEP_PIN, OUTPUT); //软驱步进

  xTaskCreate(task_step, "task_step", 2048, NULL, 1, NULL);
  xTaskCreate(task_color, "task_color", 2048, NULL, 1, NULL);
  xTaskCreate(task_melody, "task_melody", 4096, NULL, 1, NULL);
  xTaskCreate(task_play, "task_play", 4096, NULL, 1, NULL);
  xTaskCreate(task_queue, "task_queue", 2048, NULL, 1, NULL);
}