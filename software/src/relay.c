/* evse-cpc-bricklet
 * Copyright (C) 2024 Thomas Hein <github@poohnet.de>
 *
 * relay.c: Relay driver
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "relay.h"
#include "communication.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "xmc_gpio.h"

const uint8_t relay_pins[RELAY_NUM] = {9, 8};
const uint8_t relay_led_pins[RELAY_NUM] = {5, 6};

Relay relay;

void relay_set_led(const uint8_t channel, const bool value)
{
  if (channel < RELAY_NUM) {
    XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT0, relay_led_pins[channel], value ? XMC_GPIO_OUTPUT_LEVEL_LOW : XMC_GPIO_OUTPUT_LEVEL_HIGH);
  }
}

bool relay_get_value(const uint8_t channel)
{
  if (channel < RELAY_NUM) {
    return !XMC_GPIO_GetInput(XMC_GPIO_PORT0, relay_pins[channel]);
  }

  return false;
}

void relay_set_value(const uint8_t channel, const bool value)
{
  if (channel < RELAY_NUM) {
    XMC_GPIO_SetOutputLevel(XMC_GPIO_PORT0, relay_pins[channel], value ? XMC_GPIO_OUTPUT_LEVEL_LOW : XMC_GPIO_OUTPUT_LEVEL_HIGH);
  }
}

void relay_init()
{
  const XMC_GPIO_CONFIG_t gpio_config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
  };

  for (uint8_t i = 0; i < RELAY_NUM; i++) {
    XMC_GPIO_Init(XMC_GPIO_PORT0, relay_pins[i], &gpio_config);
    XMC_GPIO_Init(XMC_GPIO_PORT0, relay_led_pins[i], &gpio_config);

    relay.channel_led_config[i] = EVSE_CPC_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_OFF;
  }
}

void relay_tick()
{
  for (uint8_t i = 0; i < RELAY_NUM; i++) {
    switch (relay.channel_led_config[i]) {
      case EVSE_CPC_CHANNEL_LED_CONFIG_OFF: {
        relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_OFF;
        relay_set_led(i, false);
        break;
      }

      case EVSE_CPC_CHANNEL_LED_CONFIG_ON: {
        relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_ON;
        relay_set_led(i, true);
        break;
      }

      case EVSE_CPC_CHANNEL_LED_CONFIG_SHOW_HEARTBEAT: {
        relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_HEARTBEAT;
        led_flicker_tick(&relay.channel_led_flicker_state[i], system_timer_get_ms(), XMC_GPIO_PORT0, relay_led_pins[i]);
        break;
      }

      case EVSE_CPC_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS: {
        relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_OFF;
        relay_set_led(i, relay_get_value(i));
        break;
      }

      default: {
        break;
      }
    }
  }
}
