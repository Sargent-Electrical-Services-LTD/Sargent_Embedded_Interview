#include "exercise_app.h"

#include <stdio.h>

#include "bsp/esp-bsp.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "sensor_sim.h"

static const char *TAG = "app";

#define SAMPLE_PERIOD_MS 1000

typedef struct
{
    int value;
    bool valid;
} sensor_sample_t;

static bool sample_and_retry(uint32_t seq, sensor_sample_t *sample)
{
    if (sample == NULL)
    {
        return false;
    }

    esp_err_t last_err = ESP_FAIL;

    for (uint32_t attempt = 0; attempt < 3; attempt++)
    {
        sensor_sim_sample_t sim_sample = {
            .value = sample->value,
            .valid = sample->valid,
        };

        last_err = sensor_sim_read(seq, &sim_sample);
        sample->value = sim_sample.value;
        sample->valid = sim_sample.valid;

        if (last_err == ESP_OK)
        {
            return true;
        }

        if (last_err != ESP_OK || !sample->valid)
        {
            printf("warn: seq=%lu attempt=%lu/%lu err=%d value=%d valid=%d\n",
                   (unsigned long)seq,
                   (unsigned long)(attempt + 1),
                   (unsigned long)3,
                   (int)last_err,
                   sample->value,
                   sample->valid);
        }
    }

    return (last_err == ESP_OK);
}

void exercise_app_start(void)
{
    lv_display_t *disp = bsp_display_start();
    if (disp == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize BSP display");
        return;
    }

    ESP_ERROR_CHECK(bsp_display_backlight_on());
    ESP_LOGI(TAG, "Display initialized");

    if (!bsp_display_lock(0))
    {
        ESP_LOGE(TAG, "Failed to lock LVGL");
        return;
    }

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_center(label);
    bsp_display_unlock();

    uint32_t seq = 0;
    bool fault = false;

    while (1)
    {
        seq++;
        sensor_sample_t sample = {
            .value = 0,
            .valid = false,
        };

        bool ok = sample_and_retry(seq, &sample);
        if (!ok)
        {
            fault = true;
            ESP_LOGE(TAG,
                     "sample failed seq=%lu ticks=%lu",
                     (unsigned long)seq,
                     (unsigned long)xTaskGetTickCount());
        }
        else
        {
            fault = false;
            ESP_LOGI(TAG,
                     "sample seq=%lu value=%d ticks=%lu, valid=%d",
                     (unsigned long)seq,
                     sample.value,
                     (unsigned long)xTaskGetTickCount(),
                     sample.valid);
        }

        if (bsp_display_lock(0))
        {
            if (ok)
            {
                lv_label_set_text_fmt(label, "Value: %d\nStatus: OK", sample.value);
            }
            else if (fault)
            {
                lv_label_set_text_fmt(label, "Value: --\nStatus: FAULT");
            }
            else
            {
                lv_label_set_text_fmt(label, "Value: --\nStatus: RETRY");
            }
            bsp_display_unlock();
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}
