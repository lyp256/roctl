/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font NotoSansSC-Regular.otf --symbols 待机缺水陈循环制中超时冲洗 --format lvgl -o status_font.c
 ******************************************************************************/

#include "lvgl.h"


#ifndef STATUS_FONT
#define STATUS_FONT 1
#endif

#if STATUS_FONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+4E2D "中" */
    0x2, 0x0, 0x10, 0x0, 0x81, 0xff, 0xf8, 0x20,
    0xc1, 0x6, 0x8, 0x30, 0x41, 0xff, 0xfc, 0x10,
    0x40, 0x80, 0x4, 0x0, 0x20, 0x1, 0x0,

    /* U+51B2 "冲" */
    0x0, 0x41, 0x1, 0x6, 0x4, 0x5, 0xff, 0x4,
    0x44, 0x11, 0x10, 0x44, 0x45, 0x11, 0x37, 0xfc,
    0x91, 0x16, 0x4, 0x30, 0x10, 0x0, 0x40, 0x1,
    0x0,

    /* U+5236 "制" */
    0x68, 0x5, 0x20, 0x97, 0xf2, 0x72, 0x9, 0x8,
    0x27, 0xfe, 0x90, 0x82, 0x5f, 0xc9, 0x7f, 0x25,
    0x24, 0x94, 0x90, 0x52, 0x41, 0x4b, 0x4, 0x20,
    0x70,

    /* U+5F85 "待" */
    0x10, 0xc0, 0x83, 0x4, 0x7f, 0xa0, 0x30, 0x30,
    0xc0, 0xbf, 0xf6, 0x1, 0x38, 0x4, 0xaf, 0xfc,
    0x80, 0x42, 0x21, 0x8, 0x44, 0x20, 0x10, 0x83,
    0xc0,

    /* U+5FAA "循" */
    0x0, 0x0, 0x43, 0xf9, 0x98, 0x86, 0x21, 0x1,
    0x7f, 0xe4, 0x84, 0x19, 0x8, 0x32, 0xfe, 0xa5,
    0x4, 0x4b, 0xf8, 0x94, 0x11, 0x2f, 0xe2, 0x50,
    0x45, 0xa0, 0x8a, 0x7f, 0x0,

    /* U+65F6 "时" */
    0x0, 0x13, 0xe0, 0x48, 0x81, 0x22, 0xff, 0x88,
    0x12, 0x20, 0x4f, 0x91, 0x22, 0x64, 0x88, 0xd2,
    0x20, 0x4f, 0x81, 0x20, 0x4, 0x80, 0x10, 0x3,
    0xc0,

    /* U+673A "机" */
    0x11, 0xf0, 0x22, 0x20, 0x44, 0x47, 0xe8, 0x83,
    0x11, 0x6, 0x22, 0xe, 0x44, 0x2c, 0x88, 0x55,
    0x11, 0x22, 0x22, 0x48, 0x48, 0x90, 0x91, 0x61,
    0x22, 0x83, 0xc0,

    /* U+6C34 "水" */
    0x1, 0x0, 0x2, 0x0, 0x4, 0x0, 0x8, 0x47,
    0xd1, 0x80, 0xb6, 0x3, 0x78, 0x4, 0xa0, 0x19,
    0x20, 0x22, 0x60, 0x84, 0x63, 0x8, 0x74, 0x10,
    0x0, 0xe0, 0x0,

    /* U+6D17 "洗" */
    0x42, 0xc0, 0xcb, 0x0, 0x7f, 0x81, 0x30, 0xcc,
    0xc1, 0x83, 0x0, 0xff, 0xc0, 0x48, 0x31, 0x20,
    0x8c, 0x82, 0x22, 0x10, 0x89, 0x44, 0x26, 0x60,
    0xf0,

    /* U+73AF "环" */
    0xfb, 0xfc, 0x81, 0x82, 0x4, 0x8, 0x30, 0xf0,
    0xd0, 0x85, 0x62, 0x34, 0xc9, 0x91, 0x28, 0x41,
    0xe1, 0xc, 0x4, 0x0, 0x10, 0x0, 0x40,

    /* U+7F3A "缺" */
    0x60, 0x20, 0xfc, 0x41, 0x43, 0xf4, 0x81, 0x21,
    0x2, 0x5f, 0xc4, 0x84, 0x9, 0x8, 0xff, 0x52,
    0x30, 0xa4, 0x61, 0x49, 0x62, 0xf2, 0x46, 0x28,
    0x40, 0x30, 0x40,

    /* U+8D85 "超" */
    0x11, 0xfc, 0x20, 0xcb, 0xf9, 0x10, 0x82, 0x21,
    0x8, 0x5f, 0xf3, 0x84, 0x0, 0x28, 0xfe, 0x5f,
    0x84, 0xa3, 0x9, 0x47, 0xf5, 0x80, 0x9, 0x80,
    0x11, 0xff, 0xc0,

    /* U+9648 "陈" */
    0x0, 0x83, 0xc6, 0x9, 0x18, 0x27, 0xfe, 0xa1,
    0x2, 0x8d, 0xa, 0x24, 0x25, 0x90, 0x97, 0xfa,
    0x41, 0xb, 0x25, 0x21, 0x92, 0x8c, 0x4a, 0x21,
    0x38, 0x1c, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 256, .box_w = 13, .box_h = 14, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 23, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 48, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 73, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 98, .adv_w = 256, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 127, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 152, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 179, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 206, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 231, .adv_w = 256, .box_w = 14, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 254, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 281, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 308, .adv_w = 256, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x385, 0x409, 0x1158, 0x117d, 0x17c9, 0x190d, 0x1e07,
    0x1eea, 0x2582, 0x310d, 0x3f58, 0x481b
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 20013, .range_length = 18460, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 13, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

extern const lv_font_t lv_font_unscii_16;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t status_font = {
#else
lv_font_t status_font = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_unscii_16,
#endif
    .user_data = NULL,
};



#endif /*#if STATUS_FONT*/

