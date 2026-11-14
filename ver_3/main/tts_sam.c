#include <ctype.h>
#include <string.h>
#include "esp_log.h"
#include "tts_backend.h"
#include "sam.h"
#include "reciter.h"

/* Mirrors the main.c of the common C port of SAM:
 *   uppercase -> append '[' -> TextToPhonemes -> SetInput -> SAMMain.
 * In this port, sample count = GetBufferLength() / 50. */

bool tts_backend_render(const char *text,
                        uint8_t speed, uint8_t pitch, uint8_t mouth, uint8_t throat,
                        const uint8_t **pcm, size_t *n)
{
    char in[256];
    size_t len = strlen(text);
    if (len > 250) len = 250;
    for (size_t i = 0; i < len; i++) in[i] = (char)toupper((unsigned char)text[i]);
    in[len] = '\0';
    strcat(in, "[");

    if (!TextToPhonemes((unsigned char *)in)) return false;

    SetSpeed(speed);
    SetPitch(pitch);
    SetMouth(mouth);
    SetThroat(throat);
    SetInput(in);
    if (!SAMMain()) return false;

    *pcm = (const uint8_t *)GetBuffer();
    if (!*pcm) return false;
    size_t samples = (size_t)(GetBufferLength() / 50);
    size_t cap     = (size_t)GetBufferSize();
    if (samples > cap) {
        ESP_LOGW("tts_sam", "utterance truncated: %u > %u samples", (unsigned)samples, (unsigned)cap);
        samples = cap;
    }
    *n = samples;
    return *n > 0;
}

void tts_backend_release(void)
{
    FreeBuffer();
}

