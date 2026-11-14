#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "tts.h"
#include "audio_out.h"

#define CON_UART UART_NUM_0

static const char *PRESETS[] = {
    "Warning bell. Morning assembly starts in five minutes.",
    "Period one has commenced. Faculty please proceed to classrooms.",
    "Lunch interval has begun.",
    "Attention all personnel. Fire evacuation drill in progress.",
    "Campus bell text to speech system online.",
};

static void print_menu(void)
{
    printf("\r\nType text + Enter to speak. Commands start with '/':\r\n"
           "  /1../5 presets   /v voice   /c chime   /+ /- volume   /i memory   /? menu\r\n");
}

static void run_command(char c)
{
    if (c >= '1' && c <= '5') {
        audio_chime();
        tts_speak(PRESETS[c - '1']);
        return;
    }
    switch (c) {
    case 'v':
        tts_next_voice();
        printf("Voice: %s\r\n", tts_voice_name());
        tts_speak("Voice changed.");
        break;
    case 'c':
        audio_chime();
        break;
    case '+':
        audio_set_volume(audio_get_volume() + 10);
        printf("Volume %d%%\r\n", audio_get_volume());
        break;
    case '-':
        audio_set_volume(audio_get_volume() - 10);
        printf("Volume %d%%\r\n", audio_get_volume());
        break;
    case 'i':
        printf("Internal free: %u, PSRAM free: %u, PSRAM largest block: %u\r\n",
               (unsigned)esp_get_free_internal_heap_size(),
               (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
               (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
        break;
    default:
        print_menu();
        break;
    }
}

static void handle_line(const char *line)
{
    if (line[0] == '/') {
        run_command(line[1]);
    } else {
        tts_speak(line);
    }
}

static void console_task(void *arg)
{
    char line[256];
    size_t n = 0;
    uint8_t ch;

    print_menu();
    for (;;) {
        if (uart_read_bytes(CON_UART, &ch, 1, portMAX_DELAY) <= 0) continue;

        if (ch == '\r' || ch == '\n') {
            uart_write_bytes(CON_UART, "\r\n", 2);
            if (n) {
                line[n] = '\0';
                n = 0;
                handle_line(line);
            }
        } else if (ch == 0x08 || ch == 0x7f) {
            if (n) {
                n--;
                uart_write_bytes(CON_UART, "\b \b", 3);
            }
        } else if (ch >= 32 && ch < 127 && n < sizeof line - 1) {
            line[n++] = (char)ch;
            uart_write_bytes(CON_UART, (const char *)&ch, 1);      // local echo
        }
    }
}

void console_start(void)
{
    if (!uart_is_driver_installed(CON_UART)) {
        uart_driver_install(CON_UART, 1024, 0, 0, NULL, 0);
    }
    xTaskCreate(console_task, "console", 8192, NULL, 5, NULL);
}

