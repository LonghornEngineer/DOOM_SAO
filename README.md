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
* Passive Hardware Bus Sniffing
* User defined section for adding your own code
* High DPI ST7789 1.3" 240x240 LCD Screen
* USB Type-C configured for USB 2.0 (REVERSIBLE)

## Interfacing with the DOOM SAO via Serial UART

Connection Settings: 9600/8/N/1

Ensure your terminal terminates with Carrige Return (The Arduino Serial Monitor defaults to New Line. This is wrong)

Press 'X' to start the interactive menu.

Press 1..6 to select sub-menu options.

Press 'Q' within any sub-menu to quit to the main menu.

Menu Options:

1 - DOOM Guy Interface Mode
This allows you to modify the EEPROM directly to dynamically control the animations and SAO behavior.

2 - DOOM Guy Bus Monitor Mode
This is a filtered view of the DOOM Guy's I2C traffic. It's a poor man's logic analyzer, decoded for SAO interpretation.

3 - I2C SAO Bus Sniffer Mode
This allows you to monitor the entire I2C bus. If a host has more than one SAO port you can see what magic bytes and data other SAO's are transmitting in addtion to what the badge is using on I2C. It's a poor man's logic analyzer, decoded in general, for seeing I2C NACK, ACK, Reads, and Writes.

4 - Serial UART Sniffer Mode
This allows you to passively man in the middle another embedded system's serial UART line. 
Connect the source's serial TX to the SAO RX Pin PB23.
Connect the target's serial RX to the SAO TX Pin PB22.
Choose the appropriate baud rate and byte translation (ASCII or DEC).

5 - Custom  Application Mode
This is the area for YOU to add your own code.

6 - EEPROM Persistance Mode
This allows you to save changes made to the EEPROM, which by default reset on power cycle.

## Interfacing with the DOOM SAO via I2C

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

PROPRIETARY AND CONFIDENTIAL UNTIL AUGUST 11th, 2019 then,
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
***
