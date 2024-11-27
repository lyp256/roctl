#ifndef _OLED_H_
#define _OLED_H_
#include "lvgl.h"
typedef struct 
{
    lv_obj_t *title;
    lv_obj_t *in;
    lv_obj_t *out;
    lv_obj_t *drain;
    lv_obj_t *status;
    lv_obj_t *msg;
    lv_obj_t *uptime;
}text_ui_t;
void set_tds_ui(text_ui_t *ui, Bat3uResT *tds );
void set_status_ui(text_ui_t *ui,  char *status, char *msg);
void init_oled(text_ui_t *ui);
#endif