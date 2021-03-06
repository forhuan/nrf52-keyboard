/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

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

#include "keyboard_fn.h"
#include "../main.h"
#include "usb_comm.h"

#include "bootmagic.h"
#include "bootloader.h"
#include "eeconfig.h"
#include "host.h"
#include "keymap.h"

#ifdef NKRO_ENABLE

#ifdef BOOTMAGIC_ENABLE
extern keymap_config_t keymap_config;
#endif
/**
 * 切换nkro状态
 */
static void toggle_nkro()
{
    keyboard_nkro = !keyboard_nkro;
#ifdef BOOTMAGIC_ENABLE
    // 写入bootmagic配置
    keymap_config.nkro = keyboard_nkro;
    eeconfig_write_keymap(keymap_config.raw);
#endif
}
#endif

__attribute__((weak)) void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case KEYBOARD_CONTROL:
            switch (opt) {
            case CONTROL_SLEEP: // 睡眠
                sleep(SLEEP_MANUALLY);
                break;
            case CONTROL_NKRO: // 切换NKRO
#ifdef NKRO_ENABLE
                toggle_nkro();
#endif
                break;
            case CONTROL_BOOTLOADER: // 跳转到bootloader
                bootloader_jump();
                break;
            default:
                break;
            }
            break;

        case SWITCH_DEVICE:
            switch (opt) {
            case SWITCH_DEVICE_USB: // 切换设备
#ifdef HAS_USB
                usb_comm_switch();
#endif
                break;

            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}
