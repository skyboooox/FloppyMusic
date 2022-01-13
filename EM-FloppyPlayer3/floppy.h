#ifndef floppy_h
#define floppy_h

#include "utils.h"

#define DIR_PIN 34
#define STEP_PIN 33

unsigned long current_key = 0;
unsigned long current_hz = 0;
unsigned long last_move = 0;
float prev_hz = 0;
int steps = 100;
int dir = 1;

unsigned long current_delay = 0;

void _force_move(int dir)
{
  digitalWrite(DIR_PIN, dir);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(STEP_PIN, HIGH);
  delay(1);
}

inline void _move()
{
  steps++;
  if (steps > 80)
  {
    dir = dir == 1 ? 0 : 1;
    steps = 0;
    digitalWrite(DIR_PIN, dir);
  }
  // current_voltage = current_voltage == 1 ? 0 : 1;
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(STEP_PIN, HIGH);
}

inline void _step()
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
  current_delay = hz_to_us((unsigned long)(current_hz)*1000);
  if (micros() - last_move >= current_delay)
  {
    last_move = micros();
    _move();
  }
}

inline void _allocate()
{
  if (chn_flag[CHN_SONG])
  {
    current_key = chn_key[CHN_SONG];
    current_hz = note_freq[current_key];
    return;
  }
  if (chn_flag[CHN_TOUCH])
  {
    current_key = chn_key[CHN_TOUCH];
    current_hz = note_freq[current_key];
    return;
  }
}

void task_step(void *)
{

  while (true)
  {
    _allocate();
    _step();
    yield();
  }
}

void setup_floppy()
{
  pinMode(DIR_PIN, OUTPUT);  //软驱方向
  pinMode(STEP_PIN, OUTPUT); //软驱步进

  for (int i = 0; i < 80; i++)
  {
    _force_move(1);
  }

  xTaskCreate(task_step, "task_step", 2048, NULL, 1, NULL);
}

#endif