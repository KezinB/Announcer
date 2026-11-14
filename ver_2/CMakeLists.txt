cmake_minimum_required(VERSION 3.16)

# Restrict build to the main component and its direct requirements.
# This prevents compiling unused peripheral components (e.g. esp_lcd, lwip, mbedtls),
# avoiding GCC 15.2 Xtensa compiler bugs on esp_lcd while reducing build time significantly.
set(COMPONENTS main)

# Include ESP-IDF project build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp32s3_speaker_test)
