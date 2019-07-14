/*****************************************************************************
 * Made with beer and late nights in California.
 *
 * (C) Copyright 2017-2019 AND!XOR LLC (https://andnxor.com/).
 *
 * PROPRIETARY AND CONFIDENTIAL UNTIL AUGUST 11th, 2019 then,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ADDITIONALLY:
 * If you find this source code useful in anyway, use it in another electronic
 * conference badge, or just think it's neat. Consider buying us a beer
 * (or two) and/or a badge (or two). We are just as obsessed with collecting
 * badges as we are in making them.
 *
 * Contributors:
 *  @Cr4bf04m
 *  @andnxor
 *  @zappbrandnxor
 *  @hyr0n1
 *  @bender_andnxor
 *  @lacosteaef
 *  @f4nci3
 ****************************************************************************
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include "DG_Faces.h"
#include "MF_Logo.h"


#define TFT_CS        -1
#define TFT_RST        9
#define TFT_DC         8
#define SAOSERIAL      Serial 

int16_t dg_offset_x = 16;
int16_t dg_offset_y = 0;
int16_t dg_pixel_size = 8;
int16_t dg_rez_x = 26;

int16_t mf_offset_x = 0;
int16_t mf_offset_y = 0;
int16_t mf_pixel_size = 4;
int16_t mf_rez_x = 60;

uint8_t mem_write_address = 0x00;
uint8_t mem_write_address_valid = 0x00;

uint8_t eeprom[5] = {
  0x1B, 0x05, 0x01, 0x02, 0x64
};

//SAO mode 0 is Default, displays menu on serial interface
//SAO mode 1 is Doom Guy Control
//SAO mode 2 is I2C Sniffer - over SAO I2C bus
//SAO mode 3 is Serial Sniffer - leveraging SAMD GPIO XX for TX, GPIO XX for RX
//SAO mode 4 is Custom Application for people to drop in their own code
uint8_t sao_mode = 0;
uint8_t incomingByte = 0;
uint8_t sao_serial_incoming_byte = 0;
bool sao_serial_baud_selection = false;
uint8_t sao_serial_translation_mode = 0;
bool sao_god_mode_display = false;
bool menu_display_start = true;
bool menu_display_0 = false;
bool menu_display_1 = true;
bool menu_display_2 = true;
bool menu_display_3 = true;
bool menu_display_3_mode = true;
bool menu_display_4 = true;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void macro_splash(){
  SerialUSB.println(" ");
  SerialUSB.println("***********************************************************");
  SerialUSB.println(" ");
  SerialUSB.println("    DC27 Doom SAO - SAMD21 Arduino Compatible Dev Board");
  SerialUSB.println(" ");
  SerialUSB.println("                         ▓▓▓▓▓▓▓▓▓                         ");
  SerialUSB.println("                       ▓▓▓▓▓▓▓▓▓▓▓▓▓                       ");
  SerialUSB.println("                     ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                     ");
  SerialUSB.println("                   ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                   ");
  SerialUSB.println("                 ▓▓▓▓▓▓▓▓▓▓▓   ▓▓▓▓▓▓▓▓▓▓▓                 ");
  SerialUSB.println("               ▓▓▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓▓▓▓               ");
  SerialUSB.println("             ▓▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓▓▓▓▓▓▓▓             ");
  SerialUSB.println("           ▓▓▓▓▓▓▓▓▓▓▓               ▓▓▓▓▓▓▓▓▓▓▓           ");
  SerialUSB.println("         ▓▓▓▓▓▓▓▓▓▓▓                   ▓▓▓▓▓▓▓▓▓▓▓         ");
  SerialUSB.println("       ▓▓▓▓▓▓▓▓▓▓▓▒          ▓▓▓▓▓▓▓     ▓▓▓▓▓▓▓▓▓▓▓       ");
  SerialUSB.println("     ▓▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓   ▓▓▓▓     ▓▓▓▓▓▓▓▓▓▓▓     ");
  SerialUSB.println("   ▓▓▓▓▓▓▓▓▓▓▓             ▓▓▒     ▓▓▓       ▓▓▓▓▓▓▓▓▓▓▓   ");
  SerialUSB.println(" ▓▓▓▓▓▓▓▓▓▓▓               ▓▓▓     ▓▓▓         ▓▓▓▓▓▓▓▓▓▓▓ ");
  SerialUSB.println("▓▓▓▓▓▓▓▓▓▓        ▓▓▓▓▓▓    ▓▓▓▓▓▓▓▓▓▓▓          ▓▓▓▓▓▓▓▓▓▓");
  SerialUSB.println("▓▓▓▓▓▓▓▓         ▓▓▒  ▓▓▓          ▓▓▓▓▓▓          ▓▓▓▓▓▓▓▓");
  SerialUSB.println("▓▓▓▓▓▓▓▓         ▓▓   ▒▓▓            ▓▓▓▓▓▓          ▓▓▓▓▓▓");
  SerialUSB.println("▓▓▓▓▓▓▓▓▓▓        ▓▓▓▓▓▓▓▓▒            ▓▓▓▓▓▓          ▓▓▓▓");
  SerialUSB.println("  ▓▓▓▓▓▓▓▓▓▓          ▒▓▓▓▓▓▒           ▒▓▓▓▓▓▓         ▓▓ ");
  SerialUSB.println("    ▓▓▓▓▓▓▓▓▓▓          ▒▓▓▓▓▓▒           ▒▓▓▓▓▓▓          ");
  SerialUSB.println("      ▓▓▓▓▓▓▓▓▓▓          ▒▓▓▓▓▓▒           ▓▓▓▓▓▓▓        ");
  SerialUSB.println("        ▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓▓        ");
  SerialUSB.println("          ▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓▓▓   ▓▓▓▓▓▓▓▓▓          ");
  SerialUSB.println("            ▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓▓▓▓▓▓▓▓▓▓▓            ");
  SerialUSB.println("              ▓▓▓▓▓▓▓▓▓▓           ▓▓▓▓▓▓▓▓▓▓              ");
  SerialUSB.println("                ▓▓▓▓▓▓▓▓▓▓       ▓▓▓▓▓▓▓▓▓▓                ");
  SerialUSB.println("                  ▓▓▓▓▓▓▓▓▓▓▒ ▒▓▓▓▓▓▓▓▓▓▓                  ");
  SerialUSB.println("                    ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                    ");
  SerialUSB.println("                      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒                     ");
  SerialUSB.println("                        ▓▓▓▓▓▓▓▓▓▓▓▒                       ");
  SerialUSB.println(" ");
  SerialUSB.println("***********************************************************");
  SerialUSB.println(" ");
  delay(100); //mitigates race condition and lost println #YOLO
}

void mode_1_dg(){
  //TODO
  if(menu_display_1){
    SerialUSB.println("**Doom Guy Interface Mode**");
    SerialUSB.println("Press Q to quit back to the main menu.\n");  
    menu_display_1 = false; //this prevents infinite printing of the menu in loop   
  }
}

void mode_2_i2c_sniffer(){
  //TODO
  if(menu_display_2){
    SerialUSB.println("**I2C SAO Bus Sniffer**");
    SerialUSB.println("No Code Yet?!");
    SerialUSB.println("Press Q to quit back to the main menu.\n");  
    menu_display_2 = false; //this prevents infinite printing of the menu in loop   
  }
}

void mode_3_serial_sniffer(){
  //Since the default pads for serial are not used (RX~PB23, TX~PB22) we will use those
  //If you want to get crazy and add more serial ports visit https://learn.sparkfun.com/tutorials/adding-more-sercom-ports-for-samd-boards

  if(menu_display_3){
    SerialUSB.println("**Serial UART Sniffer**");
    SerialUSB.println("This allows you to passively man in the middle a target embedded systems serial line");
    SerialUSB.println("Connect the source's serial TX to the SAO RX Pin PB23");
    SerialUSB.println("Connect the target's serial RX to the SAO TX Pin PB22");
    SerialUSB.println("Press Q to quit back to the main menu at any time\n");
    
    SerialUSB.println("1 - 9600 baud");
    SerialUSB.println("2 - 19200 baud");
    SerialUSB.println("3 - 38400 baud");
    SerialUSB.println("4 - 57600 baud");
    SerialUSB.println("5 - 115200 baud");
    SerialUSB.print("Please select your target's baud rate 1..5: ");
    menu_display_3 = false;
  }

  if((sao_serial_baud_selection) && (sao_serial_translation_mode == 0) && (menu_display_3_mode) && (!menu_display_3)){
    SerialUSB.println("Translation of the serial data");
    SerialUSB.println("1 - Raw Decimal Bytes");
    SerialUSB.println("2 - ASCII Charachters");
    SerialUSB.print("Please select your data translation option: ");
    menu_display_3_mode = false;
  }

  if (SAOSERIAL.available() > 0) {
    sao_serial_incoming_byte = SAOSERIAL.read();
    
    if(sao_serial_translation_mode == 1)
      SerialUSB.print(sao_serial_incoming_byte); //So we can watch what is going across the serial uart bus, in decimal 
    else
      SerialUSB.print((char)sao_serial_incoming_byte); //So we can watch what is going across the serial uart bus, in ASCII
    
    SAOSERIAL.print((char)sao_serial_incoming_byte); //Pass through the what we saw, so the "intended" target gets it's serial data
  }
}

void mode_4_custom_appliaction(){
  //TODO - This section is for others to add their own code for custom functionality
  if(menu_display_4){
    SerialUSB.println("**Custom Application**");
    SerialUSB.println("No custom code added yet. How sad.");
    SerialUSB.println("To get started visit ~ https://github.com/LonghornEngineer/DOOM_SAO");
    SerialUSB.println("Press Q to quit back to the main menu.\n");  
    menu_display_4 = false; //this prevents infinite printing of the menu in loop   
  }
}

void menu(){
  switch(sao_mode){
    case 0:
      //Start message which displays in loop
      if(menu_display_start){
        SerialUSB.println(" ");
        SerialUSB.print("Press X to Start: ");
      }

      //Main menu display
      if(menu_display_0){
        macro_splash();
        SerialUSB.println("1 - DOOM Guy  Interface Mode");
        SerialUSB.println("2 - I2C SAO Bus Sniffer Mode");
        SerialUSB.println("3 - Serial UART Sniffer Mode");
        SerialUSB.println("4 - Custom  Application Mode");
        SerialUSB.print("Please Select a Mode 1, 2, 3, 4: ");  
        menu_display_0 = false; //this prevents infinite printing of the menu in loop   
      }

      incomingByte = SerialUSB.read();

      //Parse input for 'X' or 'x' to kickoff the main menu
      if(((incomingByte == 88) || (incomingByte == 120)) && (menu_display_0 == false)){
        menu_display_start = false;
        SerialUSB.print((char)incomingByte); //echo user selection to the USB terminal
        SerialUSB.println(" \n");
        menu_display_0 = true;
      }

      //If valid input 1..4 put the SAO in to that mode
      if((incomingByte > 48) && (incomingByte < 53) && (menu_display_start == false)){
        //Byte recieved is in ASCII, subtract 48 to get to the decimal value
        SerialUSB.print((char)incomingByte); //echo user selection to the USB terminal
        SerialUSB.println(" \n");
        sao_mode = incomingByte - 48;
        sao_god_mode_display = true;
        if(sao_mode == 1) mode_1_dg(); //this prevents the i2c msgs from displaying before the submenu splash
      }
      break;
      
    case 1: //DOOM Guy Interface Mode
      //No Code to add, by default this is running in the background and entering
      //This mode just toggles the println on, quitting toggles them off
      mode_1_dg();
      //Press (Q or q) to quit back to main menu
      incomingByte = SerialUSB.read();
      if((incomingByte == 81) || (incomingByte == 113)){
        sao_mode = 0;
        menu_display_0 = true;
        menu_display_1 = true;
        SerialUSB.println(" \n");
      }
      break;
    
    case 2: //I2C Sniffer Mode
      mode_2_i2c_sniffer();
      //Press (Q or q) to quit back to main menu
      incomingByte = SerialUSB.read();
      if((incomingByte == 81) || (incomingByte == 113)){
        sao_mode = 0;
        menu_display_0 = true;
        menu_display_2 = true;
      }
      break;
    
    case 3: //Serial Bus Sniffer Mode
      mode_3_serial_sniffer();
      //Press (Q or q) to quit back to main menu
      incomingByte = SerialUSB.read();
      if((incomingByte == 81) || (incomingByte == 113)){
        //Reset variables upon quitting
        sao_mode = 0;
        sao_serial_baud_selection = 0;
        sao_serial_translation_mode = 0;
        SAOSERIAL.end();
        menu_display_0 = true;
        menu_display_3 = true;
        menu_display_3_mode = true;
      }
      if((incomingByte > 48) && (incomingByte < 54) && (sao_serial_baud_selection == false)){
        switch(incomingByte){
          case 49 : SAOSERIAL.begin(9600); sao_serial_baud_selection = true; break;
          case 50 : SAOSERIAL.begin(19200); sao_serial_baud_selection = true; break; 
          case 51 : SAOSERIAL.begin(38400); sao_serial_baud_selection = true; break; 
          case 52 : SAOSERIAL.begin(57600); sao_serial_baud_selection = true; break;
          case 53 : SAOSERIAL.begin(115200); sao_serial_baud_selection = true; break;          
        }
        SerialUSB.print((char)incomingByte); //echo user selection to the USB terminal
        SerialUSB.println(" \n");
        break;
      }
      if((incomingByte > 48) && (incomingByte < 51) && (sao_serial_baud_selection == true) && (sao_serial_translation_mode == 0)) {
        switch((char)incomingByte){
          case '1': sao_serial_translation_mode = 1; break;
          case '2': sao_serial_translation_mode = 2; break;       
        }
        SerialUSB.print((char)incomingByte); //echo user selection to the USB terminal
        SerialUSB.println(" \n");
        SerialUSB.print("SERIAL SNIFFER ACTIVE... \n");
        SerialUSB.println(" \n");
      }
      break;

    case 4: //Custom Application Mode
      mode_4_custom_appliaction();
      //Press (Q or q) to quit back to main menu
      incomingByte = SerialUSB.read();
      if((incomingByte == 81) || (incomingByte == 113)){
        sao_mode = 0;
        menu_display_0 = true;
        menu_display_4 = true;
      }
      break;
  }
}

void setup(void)
{
  Wire.begin(0x50);                // Join i2c bus with address #50 to simulate AT24C32 eeprom
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);
  
  SerialUSB.begin(9600);

  tft.init(240, 240);             // Init ST7789 240x240
  tft.setRotation(2);

  tft.fillScreen(ST77XX_WHITE);
  
  render(MF_Logo, sizeof(MF_Logo)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
  delay(500);
  tft.fillScreen(ST77XX_WHITE);
}

void loop()
{
  if(sao_mode == 0){
    //Default mode - cycle through DG faces and display main menu. 
    //Assumption is this mode is normally just "plugged into a badge"
    menu();
    render(dg_normal_left, sizeof(dg_normal_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_normal_middle, sizeof(dg_normal_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_normal_right, sizeof(dg_normal_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500); 
  
    menu();
    render(dg_normal_middle, sizeof(dg_normal_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_angry_left, sizeof(dg_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_angry_middle, sizeof(dg_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_angry_right, sizeof(dg_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_damage_middle, sizeof(dg_damage_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_evil_middle, sizeof(dg_evil_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  
    menu();
    render(dg_god_mode, sizeof(dg_god_mode)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    delay(500);
  }
  else{
    //Given non default modes 1-4 imply the user is interfaced via Serial USB, they are hardware hacking.
    //It is better to disable the animation and 10x500ms delays so they have real time control of the MCU.
    //Appropriately just display God Mode in this case.
    //There is a boolean toggle because it wastes time to continually redraw the same image
    if(sao_god_mode_display) {
      render(dg_god_mode, sizeof(dg_god_mode)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      sao_god_mode_display = false;
    }
    menu();
  }
}


void render(const uint16_t pixel_array[], int16_t siz, int16_t offset_x, int16_t offset_y, int16_t pixel_size, int16_t rez_x)
{
  int16_t x_loc = 0;
  int16_t y_loc = 0;

  int16_t x = 0;
  int16_t y = 0;

  for(int i = 0; i < siz; i++)
  {    
    x = (x_loc * pixel_size) + offset_x; 
    y = (y_loc * pixel_size) + offset_y;
    tft.fillRect(x, y, pixel_size, pixel_size, pixel_array[x_loc + (y_loc * rez_x)]);
  
    x_loc++;
    if(x_loc == rez_x)
    {
      x_loc = 0;
      y_loc++;
    }
  }

  return;
}

void write_to_eeprom(uint8_t address, uint8_t value)
{
  if(sao_mode == 1){
    SerialUSB.print("Write ");
    SerialUSB.print(value,HEX);
    SerialUSB.print(" to location ");
    SerialUSB.println(address, HEX);  
  }

  // Make sure address is not in the protected area!
  if(address >= 0x04)
  {
    eeprom[address] = value;
  }
  else
  {
    if(sao_mode == 1){
      SerialUSB.print("Woah can't overwrite address: ");
      SerialUSB.println(address);
    }
  }
  return;
}

void receiveEvent(int howMany)
{
  if(sao_mode == 1){
    SerialUSB.print("receiveEvent bytes received: ");
    SerialUSB.println(howMany);
  }

  // 1 Byte Received. This is a Read From EEPROM Operation from a double i2c stop!
  if(howMany == 1)
  {
    uint8_t x = Wire.read();                 // receive byte
    mem_write_address = x;
    mem_write_address_valid = 1;
  }
  // 2 Bytes Received. This is a Write to EEPROM Operation
  else if(howMany == 2)
  {
    uint8_t eeprom_address = Wire.read();    // receive byte
    uint8_t value = Wire.read();             // receive byte
    write_to_eeprom(eeprom_address, value);
  }
  // No more Bytes. Skip it!
  else if(howMany == 0)
  {
    if(sao_mode == 1){
      SerialUSB.println("No more bytes. Skip!");
    }
  }
  // DOOM SAO does not support multi byte reads and writes
  else
  {
    if(sao_mode == 1){
      SerialUSB.println("To many bytes! Not Supported!");
    }
  }
}

void requestEvent()
{   
    // If requestEvent gets called before receiveEvent then we have a Read From EEPROM Operation from a single i2c stop!
    // This means we need to read from the i2c buffer the address!
    if(mem_write_address_valid == 0)
    {
      mem_write_address = Wire.read();
    }
    else
    {
      mem_write_address_valid = 0;
    }
    // Check to see if the location exists!
    if(mem_write_address < sizeof(eeprom))
    {
      if(sao_mode == 1){
        SerialUSB.print("requestEvent sending: 0x");
        SerialUSB.println(eeprom[mem_write_address], HEX);      
      }
      Wire.write(eeprom[mem_write_address]);
    }
    // Location doesn't exist. Return 0x00.
    else
    {
      if(sao_mode == 1){
        SerialUSB.print("Woah that is not a real address: ");
        SerialUSB.println(mem_write_address);
      }
      Wire.write(0x00);
    }
    return;
}
