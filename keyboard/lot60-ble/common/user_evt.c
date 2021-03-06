/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#include "ble_keyboard.h"
#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf_delay.h"
#include "simple_rgb/led_rgb.h"

enum keyboard_status {
    kbd_ble,
    kbd_charge,
    kbd_usb
};

static enum keyboard_status status = 0;
static bool charging_full = false;
static bool ble_connected = false;

/**
 * @brief 按状态更改 LED 样式
 * 
 */
static void led_status_change()
{
    switch (status) {
    case kbd_ble:
        keyboard_led_rgb_set(ble_connected ? 0x66ffff : 0xFFFFFF);
        break;
    case kbd_charge:
        keyboard_led_rgb_set(charging_full ? 0x00FF00 : 0xFF8000);
        break;
    case kbd_usb:
        keyboard_led_rgb_set(0x0099ff);
        break;
    default:
        break;
    }
}

void rgb_led_event_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            keyboard_led_rgb_init();
            break;
        case KBD_STATE_INITED: // 初始化完毕
            led_status_change();
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            keyboard_led_rgb_deinit();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (arg2) {
        case PWR_SAVE_ENTER: // 进入省电模式
            keyboard_led_rgb_switch(false);
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            keyboard_led_rgb_switch(true);
            break;
        default:
            break;
        }
        break;
    case USER_EVT_CHARGE: // 充电事件
        charging_full = (arg2 != BATT_CHARGING);
        led_status_change();
        break;
    case USER_EVT_USB: // USB事件
        switch (arg2) {
        case USB_WORKING:
            status = kbd_usb;
            break;
        case USB_NOT_WORKING:
        case USB_NO_HOST: // no_host状态也能说明正在充电
            status = kbd_charge;
            break;
        case USB_NOT_CONNECT:
            status = kbd_ble;
            break;
        default:
            break;
        }
        led_status_change();
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙事件
        ble_connected = (arg2 == BLE_STATE_CONNECTED);
        led_status_change();
        break;
    case USER_EVT_BLE_PASSKEY_STATE: // 请求Passkey
        switch (arg2) {
        case PASSKEY_STATE_REQUIRE:
            keyboard_led_rgb_set(0xFFFF00);
            break;
        case PASSKEY_STATE_SEND:
            keyboard_led_rgb_set(0xFF0080);
            break;
        default:
            break;
        }
        break;
    case USER_EVT_SLEEP: // 睡眠指示
        keyboard_led_rgb_direct(0b00000101);
        nrf_delay_ms(200);
        break;
    default:
        break;
    }
}

EVENT_HANDLER(rgb_led_event_handler);