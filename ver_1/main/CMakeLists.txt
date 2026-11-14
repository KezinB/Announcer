idf_component_register(
    SRCS
        "main.c"
        "console.c"
        "audio_out.c"
        "tts.c"
        "tts_sam.c"
        "text_prep.c"
        "sam/sam.c"
        "sam/render.c"
        "sam/reciter.c"
        "sam/debug.c"
    INCLUDE_DIRS "." "sam"
    REQUIRES esp_driver_i2s esp_driver_gpio esp_driver_uart esp_timer esp_system freertos log
)

# Suppress minor legacy warnings from 1982/C99 SAM codebase under modern GCC 15
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-char-subscripts
    -Wno-sign-compare
    -Wno-implicit-fallthrough
)
