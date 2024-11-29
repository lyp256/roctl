#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <sys/time.h>
#include <sys/_timeval.h>
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"
#include "pin.h"
#include "bat3u.h"
#include "oled.h"
#include "format.h"
#include "status_font.h"

static const char *TAG = "oled";

#define I2C_HOST 0
#define LCD_PIXEL_CLOCK_HZ (400 * 1000)

#define PIN_NUM_RST -1
#define I2C_HW_ADDR 0x3C

#define LCD_H_RES 128
#define LCD_V_RES 64

// Bit number used to represent command and parameter
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

void init_text_ui(lv_disp_t *disp, text_ui_t *ui)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    ui->in = lv_label_create(scr);
    ui->out = lv_label_create(scr);
    ui->drain = lv_label_create(scr);
    ui->status = lv_label_create(scr);
    ui->msg = lv_label_create(scr);
    ui->uptime = lv_label_create(scr);
    lv_obj_align(ui->status, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align(ui->msg, LV_ALIGN_TOP_MID, 0, 17);
    lv_obj_align(ui->uptime, LV_ALIGN_TOP_MID, 0, 26);
    lv_obj_align(ui->in, LV_ALIGN_TOP_LEFT, 0, 35);
    lv_obj_align(ui->out, LV_ALIGN_TOP_LEFT, 0, 44);
    lv_obj_align(ui->drain, LV_ALIGN_TOP_LEFT, 0, 53);

    lv_obj_set_style_text_font(ui->status, &status_font, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->in, &lv_font_unscii_8, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->out, &lv_font_unscii_8, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->drain, &lv_font_unscii_8, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->msg, &lv_font_unscii_8, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->uptime, &lv_font_unscii_8, LV_STATE_DEFAULT);
}

void set_tds_ui(text_ui_t *ui, Bat3uResT *tds)
{
    char inText[20];
    char outText[20];
    char drainText[20];
    char uptimeText[20];
    timeString(uptimeText, time(NULL));
    sprintf(inText, "IN: %03dppm %0.1fC", tds->Sensor1.TDS, celsius(tds->Sensor1.TDS));
    sprintf(outText, "OU: %03dppm %0.1fC", tds->Sensor3.TDS, celsius(tds->Sensor3.TDS));
    sprintf(drainText, "DR: %03dppm %0.1fC", tds->Sensor2.TDS, celsius(tds->Sensor2.TDS));

    lv_label_set_text_fmt(ui->in, "%.15s", inText);
    lv_label_set_text_fmt(ui->out, "%.15s", outText);
    lv_label_set_text_fmt(ui->drain, "%.15s", drainText);
    lv_label_set_text_fmt(ui->uptime, "%s", uptimeText);
}

void set_status_ui(text_ui_t *ui, char *status, char *msg)
{

    char uptimeText[20];
    timeString(uptimeText, time(NULL));
    lv_label_set_text_fmt(ui->status, "%.15s", status);
    lv_label_set_text_fmt(ui->msg, "%.15s", msg);
    lv_label_set_text_fmt(ui->uptime, "%s", uptimeText);
}

void init_oled(text_ui_t *ui)
{
    ESP_LOGI(TAG, "Initialize I2C bus");

    const i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = OLEDSCL,
        .sda_io_num = OLEDSDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = 1,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = I2C_HW_ADDR,
        .control_phase_bytes = 1,       // According to SSD1306 datasheet
        .lcd_cmd_bits = LCD_CMD_BITS,   // According to SSD1306 datasheet
        .lcd_param_bits = LCD_CMD_BITS, // According to SSD1306 datasheet
        .dc_bit_offset = 6,             // According to SSD1306 datasheet
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,

    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * LCD_V_RES,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }};
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_180);
    init_text_ui(disp, ui);
}
