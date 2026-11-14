#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "tts.h"
#include "tts_backend.h"
#include "text_prep.h"
#include "audio_out.h"

#define MAX_CHUNK 120

typedef struct { const char *name; uint8_t speed, pitch, mouth, throat; } voice_t;

/* Starting points. Lower pitch number = higher voice. Tune by ear. */
static const voice_t VOICES[] = {
    { "Classic SAM",    72, 64, 128, 128 },
    { "Natural Male",   78, 58, 110, 105 },
    { "Natural Female", 82, 40, 145, 145 },
    { "Campus PA",      80, 60, 128, 128 },
    { "Robot",          92, 60, 190, 190 },
};
#define NVOICES (sizeof VOICES / sizeof VOICES[0])

static const char *TAG = "tts";
static int s_voice = 3;   // Campus PA by default

void tts_init(void) { }

int tts_next_voice(void)
{
    s_voice = (s_voice + 1) % NVOICES;
    return s_voice;
}

const char *tts_voice_name(void) { return VOICES[s_voice].name; }

static void speak_chunk(const char *s, size_t len)
{
    char buf[MAX_CHUNK + 2];
    memcpy(buf, s, len);
    buf[len] = '\0';

    const voice_t *v = &VOICES[s_voice];
    const uint8_t *pcm; size_t n;
    if (!tts_backend_render(buf, v->speed, v->pitch, v->mouth, v->throat, &pcm, &n)) {
        ESP_LOGW(TAG, "render failed for: %s", buf);
        return;
    }
    audio_play_u8(pcm, n);
    tts_backend_release();
}

void tts_speak(const char *text)
{
    static char norm[512];
    text_prep(text, norm, sizeof norm);
    ESP_LOGI(TAG, "speaking: %s", norm);

    const char *p = norm;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;

        size_t n = 0, last_space = 0;
        bool hit_punct = false;
        while (p[n] && n < MAX_CHUNK) {
            char c = p[n++];
            if (c == ' ') last_space = n;
            if (c == '.' || c == ',' || c == '?') { hit_punct = true; break; }
        }
        if (!hit_punct && p[n] && last_space) n = last_space;   // cut at a word boundary

        char last = p[n - 1];
        speak_chunk(p, n);
        p += n;

        if (last == '.' || last == '?') audio_silence_ms(300);
        else if (last == ',')           audio_silence_ms(150);
    }
}

