int DIR_PIN = 33;
int SETP_PIN = 34;

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

void setup()
{
  Serial.begin(115200);
  delay(2000);

  touchSetCycles(0x0001, 0x0001);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(SETP_PIN, OUTPUT);
}

void move()
{
  steps++;
  if (steps > 80)
  {
    dir = dir == 1 ? 0 : 1;
    steps = 0;
    digitalWrite(SETP_PIN, dir);
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

int i = 0;

float hz = 100;
float hz_dir = 0.001;
int prevHz = 0;

void loop()
{
  static int all[] = {
      1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14};
  for (int i = 0; i < 13; i++)
  {
    int value = touchRead(all[i]);
    if (value > 300)
    {
      // Serial.print("touch ");
      // Serial.print(all[i]);
      // Serial.print(" = ");
      // Serial.println(value);
      hz = delays[i];
      // Serial.println(delays[i]);
    }
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

  current_hz = (int)hz;
  step();
}
