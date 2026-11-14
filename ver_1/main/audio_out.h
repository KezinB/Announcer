#pragma once
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

esp_err_t audio_init(uint32_t sample_rate);
void audio_set_volume(int percent);          // 0..100
int  audio_get_volume(void);
void audio_play_u8(const uint8_t *s, size_t n);  // 8-bit unsigned (SAM output)
void audio_silence_ms(int ms);
void audio_chime(void);                      // C5 -> E5 -> G5

