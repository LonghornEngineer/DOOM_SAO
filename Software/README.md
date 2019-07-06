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

TBD

***
**License Information**

This project is under the [Creative Commons Attribution-ShareAlike 4.0 International License](LICENSE.md). These files are provided with no warranty and should be used at your own risk. 

***
