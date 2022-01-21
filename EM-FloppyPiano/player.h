#ifndef player_h
#define player_h
#include <ArduinoJson.h>

static DynamicJsonDocument doc(40960);
static JsonArray array;
char song_name_cache[100];
int total_notes = 0;
int player_state = 0;
int player_loops = 0;

static int player_head = 0;
static long player_start_time = 0;

void _player_play(int loop = 1)
{
  player_state = 1;
  player_start_time = millis();
  player_loops = loop;
}

void player_play(const char *song_name, const char *sheet, int loop = 1)
{
  if (strcmp(song_name, song_name_cache) == 0 && player_state > 0)
  {
    return;
  }
  strcpy(song_name_cache, song_name);
  deserializeJson(doc, sheet);
  array = doc["sheet"];
  total_notes = array.size();
  _player_play(loop);
}

void player_stop(int force = 0)
{
  player_head = 0;
  player_start_time = 0;
  if (force)
  {
    player_loops = 0;
  }
  if (player_loops > 0)
  {
    player_loops--;
  }
  if (player_loops > 0 || player_loops == -1)
  {
    _player_play(player_loops);
  }
  else
  {
    player_state = 0;
    song_name_cache[0] = '\0';
  }
}

void task_player(void *)
{
  static int midi_time;
  static int midi_action;
  static int midi_tone;
  while (true)
  {
    chn_flag[CHN_SONG] = player_state;
    if (!player_state)
    {
      vTaskDelay(100);
      continue;
    }
    if (player_head >= total_notes)
    {
      player_stop();
      vTaskDelay(100);
      continue;
    }

    if (player_head == 0)
    {
      midi_time = array[player_head];
    }

    long now_time = millis();
    long deltaTime = now_time - player_start_time;
    if (deltaTime > midi_time)
    {
      midi_action = array[player_head + 1];
      midi_tone = array[player_head + 2];
      if (midi_action == 2)
      {
        chn_key[CHN_SONG] = 0;
      }
      else if (midi_action == 1)
      {
        chn_key[CHN_SONG] = midi_tone;
      }
      player_head = player_head + 3;
      midi_time = array[player_head];
    }
    vTaskDelay(1);
  }
}

void setup_player()
{
  xTaskCreate(task_player, "task_player", 4096, NULL, 1, NULL);
}

#endif