#include "esp_log.h"
#include "audio_out.h"
#include "tts.h"

void console_start(void);

void app_main(void)
{
    ESP_ERROR_CHECK(audio_init(TTS_SAMPLE_RATE));
    tts_init();
    console_start();

    audio_chime();
    tts_speak("Text to speech system online.");
}
