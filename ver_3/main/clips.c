#include <stdint.h>
#include "clips.h"
#include "audio_out.h"

/* Symbols created by EMBED_FILES in main/CMakeLists.txt (clips/<name>.pcm). */
#define CLIP_EXTERN(n) \
    extern const uint8_t _binary_##n##_pcm_start[]; \
    extern const uint8_t _binary_##n##_pcm_end[];

CLIP_EXTERN(boot)
CLIP_EXTERN(p1)
CLIP_EXTERN(p2)
CLIP_EXTERN(p3)
CLIP_EXTERN(p4)
CLIP_EXTERN(p5)

#define CLIP_ENTRY(n) { _binary_##n##_pcm_start, _binary_##n##_pcm_end }

static const struct { const uint8_t *start, *end; } CLIPS[CLIP_COUNT] = {
    [CLIP_BOOT] = CLIP_ENTRY(boot),
    [CLIP_P1]   = CLIP_ENTRY(p1),
    [CLIP_P2]   = CLIP_ENTRY(p2),
    [CLIP_P3]   = CLIP_ENTRY(p3),
    [CLIP_P4]   = CLIP_ENTRY(p4),
    [CLIP_P5]   = CLIP_ENTRY(p5),
};

void clip_play(clip_id_t id)
{
    if (id >= CLIP_COUNT) return;
    audio_play_s16le(CLIPS[id].start, (size_t)(CLIPS[id].end - CLIPS[id].start) / 2);
}
