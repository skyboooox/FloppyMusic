#ifndef midi_h
#define midi_h

void keydown(int midi_hz)
{
  current_hz = midi_hz;
  Serial.println("down");
}
void keyup()
{
  current_hz = 0;
  Serial.println("up");
}
void task_play(void *)
{
  //load song
  static DynamicJsonDocument doc(40960);
  deserializeJson(doc, json);

  static long first_timer, now_time;
  long midi_time = 1, midi_active, midi_tone;
  int midi_hz;

  JsonArray array = doc["sheet"];

  int amount_number = array.size();
  int start = 0;

  playHead = 0;
  while (true)
  {

    if (playHead == -1 || playHead >= amount_number)
    {
      first_timer = millis();
      playHead = -1;
      start = 0;
      vTaskDelay(1);
      continue;
    }

    now_time = millis();

    if ((now_time - first_timer) > midi_time || start == 0)
    {
      midi_time = doc["sheet"][playHead];
      midi_active = doc["sheet"][playHead + 1];
      midi_tone = doc["sheet"][playHead + 2];
      midi_hz = note_freq[midi_tone];
      if (midi_active == 2)
        keyup();
      if (midi_active == 1)
      {
        keydown(midi_hz);
      }
      playHead = playHead + 3;
      midi_time = doc["sheet"][playHead];
      start = 1;
      Serial.println(playHead);
    }
    vTaskDelay(1);
    // Serial.println(now_time);
  }
}

#endif