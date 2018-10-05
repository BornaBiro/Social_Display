# Social Display Arduino Library

## Description and details
This library used to control e-radionica Social Display module equipped with 32x9 LED matrix, ESP2866 with Arduino Bootloader, two IS31FL3731 Charleplexed LED controllers and a lithium battery charger.

It allows user to display message on screen, scroll it, display 8x8 bitmap image, display both message with bitmap images, connect to WLAN network, open a web page and display web page content on display.

**NOTE:** Before using this library check if you have installed:

* Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library 
* Adafruit IS31FL3731 Library: https://github.com/adafruit/Adafruit_IS31FL3731

Otherwise, this library will not work and while trying to compile examples you will get a lot of errors.

## Repository content
This repo contains Arduino Social display library (header .h file and a .cpp file). It also contains two examples:

* social_display_example - This example shows how to use basic functions of Social Display library like printing message on the screen, scrolling message, printing bitmap.
* social_display_web_example - This example shows how to connect to WiFi using this library, read some data of the web and display that data on LED matrix.

## Docs
* Product page ()
* List of functions and its parameters and returns ()

## How to use this library
Click on green "Clone or download button", then "Download ZIP", wait while file downloads, open Arduino IDE, then Sketch-> Include Library-> Add ZIP Library and then browse to your ZIP library file that you downloaded.

After that click on File-> Examples-> Social Display to use examples.

**NOTE:** Don't forget to change your board to Generic ESP8266 Board (Tools-> Boards). If you don't see this choice, that means you don't have installed ESP8266 Core For Arduino (can be found [here]).



[here]: https://github.com/esp8266/Arduino
