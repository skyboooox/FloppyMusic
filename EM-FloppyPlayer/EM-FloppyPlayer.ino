
#include <WiFi.h>

int DIR_PIN = 33;
int STEP_PIN = 34;

float hz = 100;
float hz_dir = 0.001;
int prevHz = 0;
long hz_to_us(long hz)
{
  return 2000000 * 1000 / hz;
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
      current_hz = (int)delays[max_key];
    }
    else
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