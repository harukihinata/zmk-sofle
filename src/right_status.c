#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/event_manager.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/split/bluetooth/peripheral.h>

static lv_obj_t *status_label;
static atomic_t disconnects;
static bool was_connected;

static int connection_listener(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_status_changed *event =
        as_zmk_split_peripheral_status_changed(eh);
    if (event) {
        if (was_connected && !event->connected) {
            atomic_inc(&disconnects);
        }
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
    bool connected = zmk_split_bt_peripheral_is_connected();
    /* Run all LVGL access on the display queue, below HID delivery priority. */
    lv_label_set_text_fmt(status_label,
                          "RIGHT\nBAT %u%%\nLINK %s\nUP %uh\n   %02um\nDROP %u",
                          (unsigned int)zmk_battery_state_of_charge(),
                          connected ? "OK" : "--",
                          (unsigned int)(minutes / 60),
                          (unsigned int)(minutes % 60),
                          (unsigned int)atomic_get(&disconnects));
    k_work_reschedule_for_queue(zmk_display_work_q(), &status_work, K_SECONDS(5));
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    status_label = lv_label_create(screen);
    lv_obj_set_width(status_label, lv_pct(100));
    lv_obj_set_style_text_font(status_label, &lv_font_unscii_8, LV_PART_MAIN);
    lv_obj_set_style_text_color(status_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 2, 4);
    was_connected = zmk_split_bt_peripheral_is_connected();
    update_status(NULL);
    return screen;
}
