/* evse-cpc-bricklet
 * Copyright (C) 2024 Thomas Hein <github@poohnet.de>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"
#include "relay.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

BootloaderHandleMessageResponse handle_message(const void *message, void *response)
{
  const uint8_t length = ((TFPMessageHeader*)message)->length;

  switch(tfp_get_fid_from_message(message)) {
    case FID_SET_VALUE:              return length != sizeof(SetValue)            ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_value(message);
    case FID_GET_VALUE:              return length != sizeof(GetValue)            ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_value(message, response);
    case FID_SET_CHANNEL_LED_CONFIG: return length != sizeof(SetChannelLEDConfig) ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_channel_led_config(message);
    case FID_GET_CHANNEL_LED_CONFIG: return length != sizeof(GetChannelLEDConfig) ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_channel_led_config(message, response);
    default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
  }
}

BootloaderHandleMessageResponse set_value(const SetValue *data)
{
  relay_set_value(0, data->channel0);
  relay_set_value(1, data->channel1);

  return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_value(const GetValue *data, GetValue_Response *response)
{
  response->header.length = sizeof(GetValue_Response);
  response->channel0      = relay_get_value(0);
  response->channel1      = relay_get_value(1);

  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_channel_led_config(const SetChannelLEDConfig *data)
{
  if(data->channel > 1) {
    return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
  }

  relay.channel_led_config[data->channel] = data->config;

  return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_channel_led_config(const GetChannelLEDConfig *data, GetChannelLEDConfig_Response *response)
{
  if(data->channel > 1) {
    return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
  }

  response->header.length = sizeof(GetChannelLEDConfig_Response);
  response->config        = relay.channel_led_config[data->channel];

  return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

void communication_tick()
{
  communication_callback_tick();
}

void communication_init()
{
  communication_callback_init();
}
