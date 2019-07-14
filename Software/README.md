### DOOM Shitty Add On Software 
***
**Add on for conference badges that features a LCD Screen and DOOM Guy.**

Software is written using the Arduino IDE 1.8.9. This document will explain how to setup the software environment to compile the firmware.

## Arduino Hardware Settings
* Board Manager Package: Arduino SAMD Boards (32-bits ARM Cortex-M0+) Version 1.8.2
* Board: Arduino M0

## Libraries
The following libraries need to be installed. These can be found through the Arduino Library Manager.

* Adafruit GFX Library Version 1.5.6
* Adafruit ST7735 and ST7789 Library Version 1.3.7
* Adafruit Zero DMA Library Version 1.0.4

## Modifying Libraries

To make these libraries work with the DOOM SAO Hardware some modifications must be made. 

In Windows these are located under *Documents\Arduino\libraries* if they were installed through the Arduino Library Manager.

In Ubuntu Linux these are located under *$HOME/Arduino/libraries* if they were installed through the Arduino Library Manager.

### Enabling SPI_MODE3
Since the hardware does not use the CS pin on the LCD we must change the GFX library from using SPI_MODE0 to SPI_MODE3. 

Open the *\Adafruit_GFX_Library\Adafruit_SPITFT.h* file
Change SPI_MODE0 in line 193 to SPI_MODE3
Save the file

Open the *\Adafruit_ST7735_and_ST7789_Library\Adafruit_ST7789.h* file
Change SPI_MODE0 in line 17 to SPI_MODE3
Save the file

### Enable DMA
In the GFX library the hardware DMA is not enabled by default and must be turned on.

Open the *\Adafruit_GFX_Library\Adafruit_SPITFT.h* file.
Uncomment Line 77 which reads *#define USE_SPI_DMA               ///< If set, use DMA if available*
Save the file

The DOOM_SAO.ino file should compile correctly in the Arduino IDE now. 

## Overview of the Firmware

Interactive features can be interface either through serial UART or over the I2C bus.

### Serial Interactivity

Connection Settings: 9600/8/N/1

Press 'X' to start the interactive menu.

Press 1..4 to select sub-menu options.

Press 'Q' within any sub-menu to quit to the main menu.

Menu Options:

1 - DOOM Guy Bus Monitor Mode
This allows you to see the interactions between the SAO and the host.

2 - I2C SAO Bus Sniffer Mode
This allows you to monitor the entire I2C bus. If a host has more than one SAO port you can see what magic bytes and data it is transmitting...

3 - Serial UART Sniffer Mode
This allows you to passively man in the middle another embedded system's serial UART line. 
Connect the source's serial TX to the SAO RX Pin PB23.
Connect the target's serial RX to the SAO TX Pin PB22.
Choose the appropriate baud rate and byte translation (ASCII or DEC).

4 - Custom  Application Mode
This is the area for YOU to add your own code.

### I2C Interactivity

TBD

***
**License Information**

This project is under the [Creative Commons Attribution-ShareAlike 4.0 International License](LICENSE.md). These files are provided with no warranty and should be used at your own risk. 

***
