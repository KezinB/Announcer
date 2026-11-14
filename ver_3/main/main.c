#include "esp_log.h"
#include "audio_out.h"
#include "tts.h"
#include "clips.h"

void console_start(void);

void app_main(void)
{
    ESP_ERROR_CHECK(audio_init(TTS_SAMPLE_RATE));
    tts_init();
    console_start();

    audio_chime();
    clip_play(CLIP_BOOT);   // natural voice; typed text still uses SAM
}
