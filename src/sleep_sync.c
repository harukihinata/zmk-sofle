#include <zephyr/kernel.h>
#include <zmk/activity.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_settings_changed.h>
#include <zmk/events/split_peripheral_status_changed.h>

static void sync_sleep_settings(struct k_work *work) {
    /* Read persisted values at send time, after boot/settings restoration. */
    raise_zmk_activity_settings_changed((struct zmk_activity_settings_changed){
        .idle_ms = zmk_activity_get_idle_ms(),
        .sleep_ms = zmk_activity_get_sleep_ms(),
        .source = ZMK_RELAY_EVENT_SOURCE_SELF,
    });
}

K_WORK_DELAYABLE_DEFINE(sleep_sync_work, sync_sleep_settings);

static int split_connection_listener(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_status_changed *event =
        as_zmk_split_peripheral_status_changed(eh);
    if (event && event->connected) {
        k_work_reschedule(&sleep_sync_work, K_SECONDS(2));
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(sofle_sleep_sync, split_connection_listener);
ZMK_SUBSCRIPTION(sofle_sleep_sync, zmk_split_peripheral_status_changed);
