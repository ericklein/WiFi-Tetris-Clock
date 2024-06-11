# WiFi Tetris Clock

## Contributors
This code is forked from @wene fork of original code from @witnessmenow

## Purpose
![Screenshot](readme/front_face.png)

A network connected clock that displays time using falling tetris blocks

## Features

## Target configuration
- Private access settings like WiFi SSID and password are contained in a `secrets.h` file that is not included in this repo.  Instead you'll find the file `secrets_template.h`, which should be copied to `secrets.h` and then edited to supply the right access credentials and configuration values to match your deployment environment.
- See config.h for parameter configuration

## Bill of Materials (BOM)
- MCU
    - Tested on ESP32 boards
      - Adafruit Feather Huzzah ESP32
- WiFi
    - Supported hardware
        - ESP32 based boards
- Screen
  - RGB LED Matrix 64 x 32
    - HUB75 connectors (standard)
  - LED Matrix featherboard
    - https://www.tindie.com/products/brianlough/huzzah-32-matrix-featherwing/#specs
- Power Supply
  - 5V, ?A power supply required to properly drive the LED Matrix

## Pinouts
- The breakout board handles pin and power cabling, and the underlying pinouts are described in config.h

### Power Wiring
![Screenshot](readme/wiring.png)

## Information Sources

## Issues and Feature Requests
- See GitHub Issues for project

## .plan (big ticket items)
- move NTP time code from customized library to standard ESP32 time.h