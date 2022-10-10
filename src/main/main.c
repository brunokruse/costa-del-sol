#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"

#include "discord.h"
#include "estr.h"
#include "discord/session.h"
#include "discord/voice_state.h"

static const char* TAG = "discord_bot";

static discord_handle_t bot;

// Code is modified to work with customized hardware build in GPIO_13.
// GPIO_13 triggers a switching transistor to power the connected
// usb port.

/**
 * To find out your user ID first run application
 * without setting the user id. Then join you user to
 * one of the discord voice channels and from the terminal output
 * copy the user_id and paste it here
 */

const char* VOICE_USER_ID = ""; // <--- set your discord user id here

/**
 * GPIO number of the LED that will be ON when the user is muted.
 * Otherwise, when the user is not muted, LED will be OFF.
 */

// we set this to 13 on the build because we are powering externally
// this pin is only 3.3v so we use a switching transistor to get power
// to the output from an external source.
const gpio_num_t LED_GPIO = GPIO_NUM_13;

static void bot_event_handler(void* handler_arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    discord_event_data_t* data = (discord_event_data_t*) event_data;

    switch(event_id) {
        // TODO: add a second LED to the hardware build that let's you know the bot
        // is first connected.
        case DISCORD_EVENT_CONNECTED: {
                discord_session_t* session = (discord_session_t*) data->ptr;

                ESP_LOGI(TAG, "Bot %s#%s connected",
                    session->user->username,
                    session->user->discriminator
                );
            }
            break;
        
        // respond to voice state commends from gateway
        case DISCORD_EVENT_VOICE_STATE_UPDATED: {
                discord_voice_state_t* vstate = (discord_voice_state_t*) data->ptr;

                // we probably only care about user_id, and channel_id here
                // but there are more options if we want to use them
                ESP_LOGI(TAG, "voice_state (user_id=%s, channel_id=%s, mute=%d, self_mute=%d, deaf=%d, self_deaf=%d)",
                    vstate->user_id,
                    vstate->channel_id ? vstate->channel_id : "NULL",
                    vstate->mute,
                    vstate->self_mute,
                    vstate->deaf,
                    vstate->self_deaf
                );

                if(estr_eq(VOICE_USER_ID, vstate->user_id)) {
                    if(vstate->channel_id) {
                        // send trigger to turn on LIGHT
                        gpio_set_level(LED_GPIO, 1);
                    }
                    else {
                        // send trigger to turn off LIGHT
                        gpio_set_level(LED_GPIO, 0);
                    }
                }
            }
            break;
        
        // TOOD: turn off connection light
        case DISCORD_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Bot logged out");
            break;
    }
}

void app_main(void) {
    // esp setup
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 0);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    // discord setup
    discord_config_t cfg = {
        .intents = DISCORD_INTENT_GUILD_VOICE_STATES
    };

    bot = discord_create(&cfg);
    ESP_ERROR_CHECK(discord_register_events(bot, DISCORD_EVENT_ANY, bot_event_handler, NULL));
    ESP_ERROR_CHECK(discord_login(bot));
}