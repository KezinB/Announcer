#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Render `text` to 8-bit unsigned mono PCM at 22050 Hz.
 * On success returns true and sets *pcm / *n. The buffer stays valid
 * until tts_backend_release() is called. */
bool tts_backend_render(const char *text,
                        uint8_t speed, uint8_t pitch, uint8_t mouth, uint8_t throat,
                        const uint8_t **pcm, size_t *n);
void tts_backend_release(void);

