### DOOM Shitty Add On Features
***
**Add on for conference badges that features a LCD Screen and DOOM Guy.**

The DOOM Shitty Add On (DOOM SAO) is an add on for conference badges that implement the [V1.69bis standard](https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/) for SAO devices. Additionally, it has provisions to be a full on development platform for the ATSAMD21G18A microcontroller. The MCU and preinstalled bootloader are also Arduino compatible to aid in the ease of hardware and firmware hacking. This document will explain the feature set and how to interface with the DOOM SAO to bring out its full potential.

## Full Feature Set
* Arduino Compatible MCU ATSAMD21G18A
* All unused I/O from the MCU is accessible
* Full implementation of the [V1.69bis standard](https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/)
* Auto mode for DOOM Guy for badges that are not DOOM SAO compatible
* Serial Terminal accessible through the USB Type-C connector
* High DPI ST7789 1.3" 240x240 LCD Screen
* USB Type-C configured for USB 2.0 (REVERSIBLE)

## Interfacing with the DOOM SAO

The DOOM SAO implements the [V1.69bis standard](https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/) for SAO devices and follows the [AND!XOR SAO Reference Design](https://github.com/ANDnXOR/sao-reference-designs). What this means is that using the I2C interface and GPIO pins of the V1.69bis standard users will be able to control how DOOM Guy acts and behaves.

The I2C address for DOOM SAO is 0x50. This is the standard address for the I2C AT24C32 EEPROM. Over the I2C interface, DOOM SAO acts like a simulated EEPROM. Only single byte reads and writes are supported. Attempting to do multi byte reads or writes are not implemented at this time. For identification purposes the first 3 address locations are used. See the [AND!XOR SAO Reference Design](https://github.com/ANDnXOR/sao-reference-designs) for more information. Below is the EEPROM address structure. 

| 0		| 1		| 2 	| 3...n	|
|:-:	|:-:	|:-:	|:-:	|
| 0x1B	| 0x05	| 0x01	| Data	|

* Address 0 is the DC Year. Use 0x1B for DC27.
* Address 1 is my Maker ID which is a unique identifier for SAO maker, I chose 0x05 at random for my maker ID. I do not know if there is a list of these ¯\_(ツ)_/¯.
* Address 2 is the SAO Type ID which is a unique identifier for the SAO. DOOM SAO is my only SAO so it is 0x01. 
* Addresss 3..n is where you can write stuff to the SAO!

First 3 addresses are protected and can not be overwritten unless you change them in the firmware and reflash the MCU.

### EEPROM Addresses

| Address	| Content	| Default Value	| Protected?	|
|:-:		|:-:		|:-:			|:-:			|
| 0x00		| DC Year	| 0x1B			| Yes			|
| 0x01		| Maker ID	| 0x05			| Yes			|
| 0x02		| SAO ID	| 0x01			| Yes			|
| 0x03		| AutoMode	| 0x01			| No			|
| 0x04		| Health	| 0x64			| No			|

### EEPROM functionality (quirks to note) 

Write functions to the DOOM SAO will always ACK regardless if you where successful or not in writing to that address location.
Reading data locations that do not exist will return with data 0x00. 

### What is Auto Mode?

When first booted up, DOOM SAO defaults to an auto mode. In this mode, Doom Guy looks left and right in an alternating manner with no control from the Badge. Writing a 0x00 value to this address diables auto mode and allows the GPIO to control DOOM Guy. With auto mode disabled, GPIO 1 makes DOOM Guy look left and GPIO 2 makes DOOM Guy look right. Truth table below.

| GPIO 1	| GPIO 2	| What Happens?			|
|:-:		|:-:		|:-:					|
| 0			| 0			| DG Looks Forward		|
| 1			| 0			| DG Looks Left			|
| 0			| 1			| DG Looks Right		|
| 1			| 1			| DG Looks Backwards	|

### What is Health?

***
**License Information**

This project is under the [Creative Commons Attribution-ShareAlike 4.0 International License](LICENSE.md). These files are provided with no warranty and should be used at your own risk. 

***
