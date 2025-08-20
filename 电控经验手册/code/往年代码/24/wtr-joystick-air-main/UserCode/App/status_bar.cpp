#include "status_bar.hpp"
#include "lvgl_thread.h"
#include "Battery/battery_define.hpp"
#include "Adc/adc_class_define.hpp"
#include "FreeRTOS.h"
#include "task.h"

static lv_obj_t *status_bar;
static lv_obj_t *status_bar_label;

static void StatusBar_Entry(void *argument)
{
    (void)argument;

    Adc3.Init();
    Adc3.StartDma();

    bool is_volt_red          = false;
    float voltage_threshold_l = 6.3;
    float voltage_threshold_h = 6.5;
    extern volatile uint32_t MavTotalBytesSent;
    extern volatile uint32_t MavTotalBytesGot;
    uint32_t last_mav_bytes_sent = MavTotalBytesSent;
    uint32_t last_mav_bytes_got  = MavTotalBytesGot;
    TickType_t last_tick         = xTaskGetTickCount();
    float mav_send_speed, mav_rcv_speed;

    while (1) {
        auto voltage = Batt.GetVoltage();
        if (is_volt_red) {
            if (voltage > voltage_threshold_h) {
                is_volt_red = false;
            }
        } else {
            if (voltage < voltage_threshold_l) {
                is_volt_red = true;
            }
        }

        mav_send_speed      = (MavTotalBytesSent - last_mav_bytes_sent) / 1.024f / (xTaskGetTickCount() - last_tick);
        mav_rcv_speed       = (MavTotalBytesGot - last_mav_bytes_got) / 1.024f / (xTaskGetTickCount() - last_tick);
        last_tick           = xTaskGetTickCount();
        last_mav_bytes_sent = MavTotalBytesSent;
        last_mav_bytes_got  = MavTotalBytesGot;

        LvglLock();
        if (is_volt_red) {
            lv_label_set_text_fmt(status_bar_label, "#ff0000 " LV_SYMBOL_BATTERY_EMPTY " %.2fV# Temp %.1f " LV_SYMBOL_UPLOAD " %.2fKB/s " LV_SYMBOL_DOWNLOAD " %.2fKB/s", voltage, GetCoreTemperature(), mav_send_speed, mav_rcv_speed);
        } else {
            lv_label_set_text_fmt(status_bar_label, LV_SYMBOL_BATTERY_FULL " %.2fV Temp %.1f " LV_SYMBOL_UPLOAD " %.2fKB/s " LV_SYMBOL_DOWNLOAD " %.2fKB/s", voltage, GetCoreTemperature(), mav_send_speed, mav_rcv_speed);
        }
        LvglUnlock();

        vTaskDelay(500);
    }
}

void StatusBar_Init(lv_obj_t *parent)
{
    LvglLock();
    status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, lv_pct(100), lv_pct(6));
    lv_obj_set_style_radius(status_bar, 0, 0);
    lv_obj_set_style_pad_all(status_bar, 0, 0);
    lv_obj_set_style_border_side(status_bar, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_bg_opa(status_bar, 180, 0);
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    status_bar_label = lv_label_create(status_bar);
    lv_obj_set_align(status_bar_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(status_bar_label, lv_color_make(255, 255, 255), 0);

    lv_label_set_recolor(status_bar_label, true);

    LvglUnlock();

    xTaskCreate(StatusBar_Entry, "StatusBar", 256, nullptr, 3, nullptr);
}
