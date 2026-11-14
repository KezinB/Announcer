#pragma once

#define TTS_SAMPLE_RATE 22050

void        tts_init(void);
void        tts_speak(const char *text);   // blocking
int         tts_next_voice(void);          // returns new voice index
const char *tts_voice_name(void);

