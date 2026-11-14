#include <math.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2s_pdm.h"
#include "driver/i2s_std.h"
#include "sdkconfig.h"
#include "audio_out.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifdef CONFIG_SPEAKER_PDM_GPIO
#define PDM_GPIO  ((gpio_num_t)CONFIG_SPEAKER_PDM_GPIO)
#else
#define PDM_GPIO  GPIO_NUM_18
#endif

#ifndef I2S_PDM_TX_SLOT_DAC_DEFAULT_CONFIG
#define I2S_PDM_TX_SLOT_DAC_DEFAULT_CONFIG(bits, mode) I2S_PDM_TX_SLOT_DEFAULT_CONFIG(bits, mode)
#endif

#define BLK 512

static const char *TAG = "audio";
static i2s_chan_handle_t s_tx = NULL;
static uint32_t s_rate = 22050;
static int s_vol = 70;                       // percent

esp_err_t audio_init(uint32_t sample_rate)
{
    s_rate = sample_rate;

    i2s_chan_config_t cc = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    cc.dma_desc_num  = 8;
    cc.dma_frame_num = 256;
    cc.auto_clear    = true;
    esp_err_t ret = i2s_new_channel(&cc, &s_tx, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate I2S channel: %s", esp_err_to_name(ret));
        return ret;
    }

#if defined(CONFIG_AUDIO_MODE_EXTERNAL_I2S_CONFIG)
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = (gpio_num_t)CONFIG_I2S_EXT_BCLK_GPIO,
            .ws   = (gpio_num_t)CONFIG_I2S_EXT_LRCK_GPIO,
            .dout = (gpio_num_t)CONFIG_I2S_EXT_DOUT_GPIO,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = { .mclk_inv = false, .bclk_inv = false, .ws_inv = false },
        },
    };
    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;   // same sample on L and R: full level on MAX98357A (L+R)/2 mix
    ret = i2s_channel_init_std_mode(s_tx, &std_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init standard I2S mode: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "Standard I2S mode configured at %u Hz", (unsigned)sample_rate);
#else
    i2s_pdm_tx_config_t pc = {
        .clk_cfg  = I2S_PDM_TX_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = I2S_PDM_TX_SLOT_DAC_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                       I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk  = I2S_GPIO_UNUSED,
            .dout = PDM_GPIO,
            .invert_flags = { .clk_inv = false },
        },
    };
    ret = i2s_channel_init_pdm_tx_mode(s_tx, &pc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init PDM mode: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "PDM TX on GPIO %d at %u Hz", (int)PDM_GPIO, (unsigned)sample_rate);
#endif

    ret = i2s_channel_enable(s_tx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable I2S channel: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

void audio_set_volume(int p) { s_vol = p < 0 ? 0 : (p > 100 ? 100 : p); }
int  audio_get_volume(void)  { return s_vol; }

static void write_block(const int16_t *p, size_t n)
{
    if (!s_tx || !p || n == 0) return;
    size_t w = 0;
    i2s_channel_write(s_tx, p, n * sizeof(int16_t), &w, portMAX_DELAY);
}

void audio_play_u8(const uint8_t *s, size_t n)
{
    if (!s || n == 0) return;
    int16_t blk[BLK];
    int32_t gain = (s_vol * 256) / 100;      // Q8 gain
    for (size_t i = 0; i < n; ) {
        size_t m = (n - i < BLK) ? (n - i) : BLK;
        for (size_t k = 0; k < m; k++) {
            int32_t v = ((int32_t)s[i + k] - 128) << 8;   // unsigned 8-bit -> signed 16-bit
            blk[k] = (int16_t)((v * gain) >> 8);
        }
        write_block(blk, m);
        i += m;
    }
}

void audio_silence_ms(int ms)
{
    int16_t blk[BLK] = {0};
    size_t n = (size_t)s_rate * ms / 1000;
    for (size_t i = 0; i < n; ) {
        size_t m = (n - i < BLK) ? (n - i) : BLK;
        write_block(blk, m);
        i += m;
    }
}

static void tone(float freq, int ms, float decay)
{
    int16_t blk[BLK];
    size_t n = (size_t)s_rate * ms / 1000;
    float g = s_vol / 100.0f;
    for (size_t i = 0; i < n; ) {
        size_t m = (n - i < BLK) ? (n - i) : BLK;
        for (size_t k = 0; k < m; k++) {
            float t = (float)(i + k) / (float)s_rate;
            blk[k] = (int16_t)(sinf(2.0f * (float)M_PI * freq * t) * expf(-decay * t) * 12000.0f * g);
        }
        write_block(blk, m);
        i += m;
    }
}

void audio_chime(void)
{
    tone(523.25f, 300, 5.0f);   // C5
    tone(659.25f, 300, 5.0f);   // E5
    tone(783.99f, 500, 4.0f);   // G5
}

