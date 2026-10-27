#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/event_manager.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#include "peripheral_status.h"

/* Keep nice!view's native status canvas; replace only its art image. */
static struct zmk_widget_status status_widget;
static lv_obj_t *stats_art;
static atomic_t disconnects;
static bool was_connected;
static uint8_t pixels[92 * 68] __aligned(4);
static const lv_image_dsc_t stats_image = {
    .header = {.magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_L8,
               .w = 92, .h = 68, .stride = 92},
    .data_size = sizeof(pixels),
    .data = pixels,
};

struct glyph { char key; uint8_t columns[5]; };
static const struct glyph glyphs[] = {
    {'0', {0x3e, 0x51, 0x49, 0x45, 0x3e}},
    {'1', {0x00, 0x42, 0x7f, 0x40, 0x00}},
    {'2', {0x42, 0x61, 0x51, 0x49, 0x46}},
    {'3', {0x21, 0x41, 0x45, 0x4b, 0x31}},
    {'4', {0x18, 0x14, 0x12, 0x7f, 0x10}},
    {'5', {0x27, 0x45, 0x45, 0x45, 0x39}},
    {'6', {0x3c, 0x4a, 0x49, 0x49, 0x30}},
    {'7', {0x01, 0x71, 0x09, 0x05, 0x03}},
    {'8', {0x36, 0x49, 0x49, 0x49, 0x36}},
    {'9', {0x06, 0x49, 0x49, 0x29, 0x1e}},
    {'D', {0x7f, 0x41, 0x41, 0x22, 0x1c}},
    {'E', {0x7f, 0x49, 0x49, 0x49, 0x41}},
    {'G', {0x3e, 0x41, 0x49, 0x49, 0x7a}},
    {'H', {0x7f, 0x08, 0x08, 0x08, 0x7f}},
    {'I', {0x00, 0x41, 0x7f, 0x41, 0x00}},
    {'M', {0x7f, 0x02, 0x0c, 0x02, 0x7f}},
    {'O', {0x3e, 0x41, 0x41, 0x41, 0x3e}},
    {'P', {0x7f, 0x09, 0x09, 0x09, 0x06}},
    {'R', {0x7f, 0x09, 0x19, 0x29, 0x46}},
    {'S', {0x46, 0x49, 0x49, 0x49, 0x31}},
    {'T', {0x01, 0x01, 0x7f, 0x01, 0x01}},
    {'U', {0x3f, 0x40, 0x40, 0x40, 0x3f}},
};

static void draw_text(int x, int y, const char *text) {
    for (; *text && x + 5 <= 68; ++text, x += 6) {
        for (size_t g = 0; g < ARRAY_SIZE(glyphs); ++g) {
            if (glyphs[g].key != *text) { continue; }
            for (int col = 0; col < 5; ++col) {
                for (int row = 0; row < 7 && y + row < 92; ++row) {
                    if (glyphs[g].columns[col] & BIT(row)) {
                        /* Match the native status canvas's 270-degree rotation. */
                        pixels[(67 - x - col) * 92 + y + row] = 0;
                    }
                }
            }
            break;
        }
    }
}

static int connection_listener(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_status_changed *event =
        as_zmk_split_peripheral_status_changed(eh);
    if (event) {
        if (was_connected && !event->connected) { atomic_inc(&disconnects); }
        was_connected = event->connected;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(sofle_right_status, connection_listener);
ZMK_SUBSCRIPTION(sofle_right_status, zmk_split_peripheral_status_changed);

static void update_status(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(status_work, update_status);

static void update_status(struct k_work *work) {
    uint64_t minutes = k_uptime_get() / 60000;
    char text[24];
    memset(pixels, 0xff, sizeof(pixels));
    draw_text(4, 6, "RIGHT");
    draw_text(4, 24, "UPTIME");
    snprintf(text, sizeof(text), "%uH %02uM", (unsigned int)(minutes / 60),
             (unsigned int)(minutes % 60));
    draw_text(4, 36, text);
    draw_text(4, 56, "DROPS");
    snprintf(text, sizeof(text), "%u", (unsigned int)atomic_get(&disconnects));
    draw_text(4, 68, text);
    lv_image_cache_drop(&stats_image);
    lv_image_set_src(stats_art, &stats_image);
    k_work_reschedule_for_queue(zmk_display_work_q(), &status_work, K_SECONDS(5));
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    zmk_widget_status_init(&status_widget, screen);
    lv_obj_t *widget = zmk_widget_status_obj(&status_widget);
    lv_obj_align(widget, LV_ALIGN_TOP_LEFT, 0, 0);
    stats_art = lv_obj_get_child(widget, 1);
    was_connected = zmk_split_bt_peripheral_is_connected();
    update_status(NULL);
    return screen;
}
