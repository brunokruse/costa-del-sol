# costa-del-sol

An esp32 based discord bot light-like. On the shoulders of giants; project built off [abobija's esp32-discord](https://github.com/abobija/esp-discord) library written in C.

<br>

# about
[esp32](https://www.espressif.com/en/products/modules/esp32) is a microcrontroller (kinda like arduino) manufactured by espressif.

The bot lives on the microcontroller and uses built-in WiFi to connect to discord.

This v1 version is hardcoded to connect to a guild server and a specific user. The bot must be invited to the server and allowed permission to see 'voice-state'. 

<br>

#  how it works

The bot first connects to wifi then attempts to create a socket connection to the [discord gateway api](https://discord.com/developers/docs/topics/gateway) to receive 'voice-state' commands from the desired user. After the connection is made, it only listens to discord events when the user joins/exits a channel.

If the user voice-state object contains a channelID (meaning it is not null) they are in a voice channel. As of now we don't care what channel specifically, just that they are in one.

### example return:
```
{
  "channel_id": "157733188964188161",
  "user_id": "80351110224678912",
  "session_id": "90326bd25d71d39b9ef95b299e3872ff",
  "deaf": false,
  "mute": false,
  "self_deaf": false,
  "self_mute": true,
  "suppress": false,
  "request_to_speak_timestamp": "2021-03-31T18:45:31.297561+00:00"
}
```

More information is available on the [discord bot developer portal](https://discord.com/developers/docs/intro). 

<br>

# setup

0. configure dev environment, espressif SDK and esp32 tool chain. more info at:

    `https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/`


1. configure wifi address and discord bot key

    `idf.py menuconfig`


2. after making changes rebuild

    `idf.py build`

3. flash

    `idf.py flash --port /dev/tty.usbserial-0001`

<br>

# More info

## contact
https://brunokruse.com/
https://github.com/brunokruse

<br>

credit to esp32-discord library by:
https://github.com/abobija/esp-discord

<br>

## license
[MIT](https://opensource.org/licenses/MIT)

