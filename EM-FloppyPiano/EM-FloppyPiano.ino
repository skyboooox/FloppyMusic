
#include <WiFi.h>

int delays[] = {
    131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};

long hz_to_us(long hz)
{
  return 1000000 / hz;
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
    digitalWrite(34, dir);
  }
  // current_voltage = current_voltage == 1 ? 0 : 1;
  digitalWrite(33, LOW);
  digitalWrite(33, HIGH);
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
  static long last_change = 0;
  static long index = 0;
  while (true)
  {
    if (millis() - last_change > 500)
    {
      index++;
      if (index > 15)
        index = 0;
      last_change = millis();
    }
    current_hz = delays[index];
    vTaskDelay(1);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);
  xTaskCreate(task_step, "task_step", 2048, NULL, 2, NULL);
  xTaskCreate(task_melody, "task_melody", 4096, NULL, 4, NULL);
}

void loop()
{
}

// int i = 0;

// float hz = 100;
// float hz_dir = 0.001;
// int prevHz = 0;
// void loop()
// {

//   // current_hz = delays[(millis() / 300) % 15];
//   // if (prevHz != current_hz)
//   // {
//   //   Serial.println(current_hz);
//   // }
//   // prevHz = current_hz;
//   if (hz > 300)
//   {
//     hz_dir = -0.001;
//   }
//   else if (hz < 100)
//   {
//     hz_dir = 0.001;
//   }
//   hz += hz_dir;
//   current_hz = (int)hz;
//   // yield();
//   // // put your main code here, to run repeatedly:
//   // for (size_t j = 0; j < 10; j++)
//   // {
//   //   for (int i = 0; i < 5; i++)
//   //   {
//   //     digitalWrite(16, HIGH);
//   //     digitalWrite(16, LOW);
//   //     delay(10);
//   //     Serial.print("A");
//   //   }
//   //   Serial.print("\n");
//   //   digitalWrite(17, HIGH);
//   // }

//   // // for (int i = 0; i < 20; i++)
//   // // {
//   // //   digitalWrite(16, HIGH);
//   // //   delay(2);
//   // //   digitalWrite(16, LOW);
//   // //   delay(5);
//   // //   Serial.print(i);
//   // // }

//   // // digitalWrite(17, HIGH);
//   // // delay(10);
//   // for (size_t j = 0; j < 10; j++)
//   // {
//   //   for (int i = 0; i < 5; i++)
//   //   {
//   //     digitalWrite(16, HIGH);
//   //     digitalWrite(16, LOW);
//   //     delay(10);
//   //     Serial.print("B");
//   //   }
//   //   Serial.print("\n");
//   //   digitalWrite(17, LOW);
//   // }

//   // // delay(10);
//   // // digitalWrite(16, HIGH);
//   // // delay(5);
//   // // digitalWrite(16, LOW);
//   // // delay(500);
// }
