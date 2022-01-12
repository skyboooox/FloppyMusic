
#include <WiFi.h>

int DIR_PIN = 33;
int STEP_PIN = 34;

float hz = 100;
float hz_dir = 0.001;
int prevHz = 0;
long hz_to_us(long hz)
{
  return 2000000 / hz;
}

long last_move = 0;
long current_hz = 0;
int current_voltage = 0;

int steps = 100;
int dir = 1;

void move()
{
  steps++;
  if (steps > 60)
  {
    dir = dir == 1 ? 0 : 1;
    steps = 0;
    digitalWrite(STEP_PIN, dir);
  }
  // current_voltage = current_voltage == 1 ? 0 : 1;
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
}

void step()
{
  if (current_hz == 0)
    return;

  long d = hz_to_us(current_hz);
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

void task_melody(void *)
{
  static int all[] = {
      1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14};

  static int delays[] = {
      130, 147, 164, 174, 196, 220, 246, 138, 155, 185, 207, 233};

  static long last_change = 0;
  static long index = 0;
  while (true)
  {
    int empty = 1;
    for (int i = 0; i < 12; i++)
    {
      int value = touchRead(all[i]);
      yield();
      if (value > 300)
      {
        current_hz = (int)delays[i];
        empty = 0;
        break;
      }
    }
    if (empty)
    {
      current_hz = 0;
    }
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

  touchSetCycles(0x0005, 0x0005);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  xTaskCreate(task_step, "task_step", 2048, NULL, 1, NULL);
  xTaskCreate(task_melody, "task_melody", 4096, NULL, 1, NULL);
}

// if (hz > 300)
// {
//   hz_dir = -0.001;
// }
// else if (hz < 100)
// {
//   hz_dir = 0.001;
// }
// hz += hz_dir;