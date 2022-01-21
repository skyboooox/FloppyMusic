#include <amidino.h>

#define LED_PIN 20

MidiParser midi;

void handleNoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
  digitalWrite(LED_PIN, (vel > 0) ? HIGH : LOW);
}

void handleNoteOff(uint8_t ch, uint8_t note, uint8_t vel)
{
  handleNoteOn(ch, note, 0);
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  midi.setChannel(MIDI_OMNI);
  midi.setNoteOnHandler(handleNoteOn);
  midi.setNoteOffHandler(handleNoteOff);
}

void loop()
{
  while (Serial.available())
  {
    midi.parse(Serial.read());
  }
}
