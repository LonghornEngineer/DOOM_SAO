/*****************************************************************************
 * Made with beer and late nights in California and Texas.
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
 
#include <Adafruit_GFX.h>   
#include <Adafruit_ST7789.h> 
#include <Wire.h>
#include "FlashStorage.h"
#include "DG_Faces.h"
#include "MF_Logo.h"

#define TFT_CS        -1
#define TFT_RST        9
#define TFT_DC         8
#define GPIO1          6 
#define GPIO2          5
#define SAOSERIAL      Serial 

#define SAMPLE            (REG_PORT_IN0 & (PORT_PA22 | PORT_PA23))
#define START1            (PORT_PA22 | PORT_PA23)
#define START2             PORT_PA23

#define BUFFSIZE          5000
#define TIMEOUT           1000

uint32_t buffer[BUFFSIZE];

int16_t dg_offset_x = 16;
int16_t dg_offset_y = 0;
int16_t dg_pixel_size = 8;
int16_t dg_rez_x = 26;

int16_t mf_offset_x = 0;
int16_t mf_offset_y = 0;
int16_t mf_pixel_size = 4;
int16_t mf_rez_x = 60;

uint8_t automode_cycle = 0x00;
uint8_t cycle = 0x00;
uint8_t mem_write_address = 0x00;
uint8_t mem_write_address_valid = 0x00;

// Simulated EEPROM address and storage!
//  Address | Content
//  0x00    | DC Year
//  0x01    | Maker ID
//  0x02    | SAO ID
//  0x03    | AutoMode
//  0x04    | Health
//  0x05    | Anger
uint8_t eeprom[6] = 
{
  0x1B, 0x05, 0x01, 0x01, 0x64, 0x00
};

// Create a structure to store our virtual EEPROM data to internal program flash
typedef struct
{
  // This by default will be false and set the first time the SAO is powered up
  bool init;        
  bool m6_persistence; 
  bool m7_airplane;
  // We don't store eeprom 0..2 because they are protected anyway
  uint8_t eeprom_3; 
  uint8_t eeprom_4;
  uint8_t eeprom_5;
} sao_data;

long previous_write_time = 0;

// Instanciate the flash store
FlashStorage(sao_flash_store, sao_data);
sao_data data;

bool menu_display_start = true;
bool menu_display_0 = false;
bool menu_display_1 = true;
bool menu_display_1_1 = true;
bool menu_display_2 = true;
bool menu_display_3 = true;
bool menu_display_4 = true;
bool menu_display_4_1 = true;
bool menu_display_5 = true;
bool menu_display_6 = true;
bool menu_display_7 = true;

bool sao_god_mode_display = false;
uint8_t sao_mode = 0;
uint8_t incomingByte = 0;

uint8_t m1_interface_addr = 0;
uint8_t m1_interface_value = 0;
int m1_char_count = 0;
int m1_inChar = 0;
int m1_inconspicuous_variable = 0;
int m1_lowkey_variable = 0;
bool m1_interface_addr_selection = false;
bool m1_interface_value_selection = false;
String m1_inString = ""; 

bool m4_hw_baud_selection = false;
uint8_t m4_hw_translation_mode = 0;
uint8_t m4_hw_incoming_byte = 0;

bool m6_persistence = false;
bool m6_persistence_init_test = false;

bool m7_airplane = false;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void macro_splash()
{
  SerialUSB.println(" ");
  SerialUSB.println("***********************************************************");
  SerialUSB.println(" ");
  SerialUSB.println("    DC27 DOOM SAO - SAMD21 Arduino Compatible Dev Board");
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
  SerialUSB.flush();
}

void persist_eeprom()
{
  sao_flash_store.write(data);
  return;
}

// This function converts integers to hex for pretty printing  
void printHex(int num, int precision)
{
  char tmp[16];
  char format[128];
  sprintf(format, "0x%%.%dX", precision);
  sprintf(tmp, format, num);
  SerialUSB.print(tmp);
  return;
}

void mode_1_dg_interface()
{
  if(menu_display_1)
  {
    SerialUSB.println("\n**DOOM Guy Interface Mode**");
    SerialUSB.println("Press Q to quit back to the main menu");  
    SerialUSB.println("For details visit ~ https://github.com/LonghornEngineer/DOOM_SAO");
    SerialUSB.println("-----------------------------------------------------------------");  
    SerialUSB.println("|EEPROM ADDRESS|    0 |    1 |    2 |    3 |    4 |    5 | 6..n |");
    SerialUSB.println("-----------------------------------------------------------------");
    SerialUSB.print("|CURRENT  VALUE| ");
    for(int i = 0; i < 6; i++){
      printHex(eeprom[i],2);  
      SerialUSB.print(" | ");
    }
    SerialUSB.println("???? |");    
    SerialUSB.println("-----------------------------------------------------------------");
    SerialUSB.println("|DEFAULT  VALUE| 0x1B | 0x05 | 0x01 | 0x01 | 0x64 | 0x00 | ???? |");
    SerialUSB.println("-----------------------------------------------------------------");
    SerialUSB.println("*Address 0..2 Are Protected");
    SerialUSB.println("*Address 3 is AutoMode Toggle");
    SerialUSB.println("*Address 4 is Health");
    SerialUSB.println("*Address 5 is Anger");
    SerialUSB.println("*Address 6..255 Are Sekret Data");
    SerialUSB.print("Please select your target address (3..255): ");
    menu_display_1 = false; //this prevents infinite printing of the menu in loop   
  }

  if((m1_interface_addr_selection) && (!m1_interface_value_selection) && (menu_display_1_1) && (!menu_display_1))
  {
    SerialUSB.print("Please enter the new value for this address in decimal (255 max): ");
    menu_display_1_1 = false;
  }

  // Clear the Buffers
  if(m1_inString[m1_char_count] == '\r')
  {
    m1_inString = ""; // clear the buffer for new input if they hit return
    m1_char_count = 0;
  }

  // This parses the input for ASCII numbers, charachters will be ignored (except Q or q for Quit)
  while (SerialUSB.available() > 0)
  {
    m1_inChar = SerialUSB.read();
    if (isAlpha(m1_inChar))
    {
      if(((char)m1_inChar == 'q') || ((char)m1_inChar == 'Q'))
      {
        // Press (Q or q) to quit back to main menu
        SerialUSB.print((char)m1_inChar); //echo user selection to the USB terminal
        SerialUSB.println(" \n"); 
        sao_mode = 0;
        menu_display_0 = true;
        menu_display_1 = true;
        menu_display_1_1 = true;
        break;      
      }
    }
    
    if (isDigit(m1_inChar))
    {
      // convert the incoming byte to a char and add it to the string if it happens to be a digit
      m1_inString += (char)m1_inChar;
      m1_char_count++;
    }
    
    if ((m1_inChar == '\r') && (m1_inString.length() > 0))
    {
      // if you get a carrige return, parse the input
      if(!m1_interface_addr_selection)
      {
        // Assign the address
        if(m1_inString.toInt() <= 255)
          m1_interface_addr = m1_inString.toInt();
        else
          m1_interface_addr = 255;
        
        m1_interface_addr_selection = true;
        //Echo User Input to the Terminal
        SerialUSB.println(m1_inString);
      }
      else if((m1_interface_addr_selection)&&(!m1_interface_value_selection))
      {
        // Assign the value
        if(m1_inString.toInt() <= 255)
          m1_interface_value = m1_inString.toInt();
        else
          m1_interface_value = 255;
        m1_interface_value_selection = true;
        // Echo User Input to the Terminal
        SerialUSB.println(m1_inString);
      }
      
      if((m1_interface_addr_selection)&&(m1_interface_value_selection))
      {           
        // Update the EEPROM
        write_to_eeprom(m1_interface_addr, m1_interface_value);
        
        // Reset Variables so the interactive menu appears again
        m1_interface_addr_selection = false;
        m1_interface_value_selection = false;
        menu_display_1 = true;
        menu_display_1_1 = true;
        m1_char_count = 0;
      }
      // clear the buffer for new input
      m1_inString = ""; 
    }
  }    
}

void mode_2_dg_sniffer()
{
  if(menu_display_2)
  {
    SerialUSB.println("**DOOM Guy Bus Sniffer Mode**");
    if(m7_airplane) SerialUSB.println("Currently in Airplane Mode - You Won't See Anything!");
    SerialUSB.println("Press Q to quit back to the main menu.\n");  
    // this prevents infinite printing of the menu in loop 
    menu_display_2 = false;   
  }
  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    sao_mode = 0;
    menu_display_0 = true;
    menu_display_2 = true;
    SerialUSB.println(" \n");
  }
}

//////////////////////////
// i2c sniffer based off the code by rricharz
// https://github.com/rricharz/i2c-sniffer-100kBaud-Arduino-Mega
// Rene Richarz if you are out there. I will buy you all the beer! @Cr4bf04m
//////////////////////////

void mode_3_i2c_sniffer()
{
  if(menu_display_3)
  {
    SerialUSB.println("**I2C SAO Bus Sniffer**");
    SerialUSB.println("Based off code by rricharz");
    SerialUSB.print("Press Q to quit back to the main menu: ");  
    // this prevents infinite printing of the menu in loop   
    menu_display_3 = false; 
  }
  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    // echo user selection to the USB terminal
    SerialUSB.print((char)incomingByte); 
    SerialUSB.println(" \n");
    sao_mode = 0;
    menu_display_0 = true;
    menu_display_3 = true;
  }
  int points = acquire_data();
  display_data(points);
}

void mode_4_serial_sniffer()
{
  // Since the default pads for serial are not used (RX~PB23, TX~PB22) we will use those
  // If you want to get crazy and add more serial ports visit https://learn.sparkfun.com/tutorials/adding-more-sercom-ports-for-samd-boards
  if(menu_display_4)
  {
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
    menu_display_4 = false;
  }

  if((m4_hw_baud_selection) && (m4_hw_translation_mode == 0) && (menu_display_4_1) && (!menu_display_4))
  {
    SerialUSB.println("Translation of the serial data");
    SerialUSB.println("1 - Raw Decimal Bytes");
    SerialUSB.println("2 - ASCII Charachters");
    SerialUSB.print("Please select your data translation option: ");
    menu_display_4_1 = false;
  }

  if (SAOSERIAL.available() > 0) 
  {
    m4_hw_incoming_byte = SAOSERIAL.read();
    
    if(m4_hw_translation_mode == 1)
    {
      // So we can watch what is going across the serial uart bus, in decimal 
      SerialUSB.print(m4_hw_incoming_byte); 
    }
    else
    {
      // So we can watch what is going across the serial uart bus, in ASCII
      SerialUSB.print((char)m4_hw_incoming_byte); 
    }
    // Pass through the what we saw, so the "intended" target gets it's serial data
    SAOSERIAL.print((char)m4_hw_incoming_byte); 
  }

  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    // Reset variables upon quitting
    sao_mode = 0;
    m4_hw_baud_selection = 0;
    m4_hw_translation_mode = 0;
    SAOSERIAL.end();
    menu_display_0 = true;
    menu_display_4 = true;
    menu_display_4_1 = true;
  }
  if((incomingByte >= '1') && (incomingByte <= '5') && (m4_hw_baud_selection == false))
  {
    switch(incomingByte)
    {
      case '1' : SAOSERIAL.begin(9600);   m4_hw_baud_selection = true; break;
      case '2' : SAOSERIAL.begin(19200);  m4_hw_baud_selection = true; break; 
      case '3' : SAOSERIAL.begin(38400);  m4_hw_baud_selection = true; break; 
      case '4' : SAOSERIAL.begin(57600);  m4_hw_baud_selection = true; break;
      case '5' : SAOSERIAL.begin(115200); m4_hw_baud_selection = true; break;          
    }
    SerialUSB.print((char)incomingByte); //echo user selection to the USB terminal
    SerialUSB.println(" \n");
  }
  else if(((incomingByte == '1') || (incomingByte == '2')) && (m4_hw_baud_selection == true) && (m4_hw_translation_mode == 0))
  {
    switch((char)incomingByte)
    {
      case '1':
        m4_hw_translation_mode = 1;
        break;
      case '2':
        m4_hw_translation_mode = 2;
        break;       
    }
    // echo user selection to the USB terminal
    SerialUSB.print((char)incomingByte); 
    SerialUSB.println(" \n");
    SerialUSB.print("SERIAL SNIFFER ACTIVE... \n");
    SerialUSB.println(" \n");
  }
}

void mode_5_custom_appliaction()
{
  // TODO - This section is for others to add their own code for custom functionality
  if(menu_display_5)
  {
    SerialUSB.println("**Custom Application**");
    SerialUSB.println("No custom code added yet. How sad.");
    SerialUSB.println("To get started visit ~ https://github.com/LonghornEngineer/DOOM_SAO");
    SerialUSB.print("Press Q to quit back to the main menu: ");  
    // this prevents infinite printing of the menu in loop   
    menu_display_5 = false; 
  }
  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    // echo user selection to the USB terminal
    SerialUSB.print((char)incomingByte); 
    SerialUSB.println(" \n");
    sao_mode = 0;
    menu_display_0 = true;
    menu_display_5 = true;
  }
}

void mode_6_persistence()
{
  if(menu_display_6)
  {
    SerialUSB.println("**Persistence Mode**");
    SerialUSB.println("This allows you to enable or disable the saving of your EEPROM writes to internal flash.");
    SerialUSB.println("Ex: You change the DG health from the Interface Mode and want the new animation to persist.");
    SerialUSB.println("DISCLAIMER: Flash memory has a limited amount of write cycles!");
    SerialUSB.println("Typical flash memory can perform about 10000 writes cycles to the same block.");
    SerialUSB.println("Then it will wear out and begin to lose it's ability to retain data.");
    SerialUSB.println("You may think, you are just changing the health value a few times and it's no big deal.");
    SerialUSB.println("What if you write your own custom application to enumerate SAO id's to look for unlocks?");
    SerialUSB.println("We reccomend you only enable persistence when you REQUIRE changes being written to EEPROM.");
    SerialUSB.println("Then disable it once you are done.\n");

    if(m6_persistence) SerialUSB.println("Persistence is currently: ENABLED\n");
    else SerialUSB.println("Persistence is currently: DISABLED\n");

    //This should only display the very first time after a firmware flash. Once power cycled it should never print.
    if(m6_persistence_init_test)SerialUSB.println("The SAO Flash was initialized when the badge turned on.\n");

    SerialUSB.println("Press Q : Quit Back to the Main Menu");  
    SerialUSB.println("Press 1 : DISABLE Persistence (default)"); 
    SerialUSB.println("Press 2 : ENABLE Persistence"); 
    SerialUSB.print("Please enter a selection: "); 
    menu_display_6 = false; //this prevents infinite printing of the menu in loop   
  }

  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    sao_mode = 0;
    menu_display_0 = true;
    menu_display_6 = true;
  }

  // m6_persistence Selection
  if((incomingByte == '1') || (incomingByte == '2')){
    if(incomingByte == '1'){//Disable Persisance
      if(m6_persistence)
      {
        // Only change it if it's different, so here its currently TRUE and they want it FALSE
        data.m6_persistence = false;
        persist_eeprom();
        m6_persistence = false;
      }
    }
    else
    {
      // Enable m6_persistence
      // Only change it if it's different, so here its currently FALSE and they want it TRUE
      if(!m6_persistence){
        data.m6_persistence = true;
        persist_eeprom();
        m6_persistence = true;
      }
    }
    // echo user selection to the USB terminal
    SerialUSB.print((char)incomingByte); 
    SerialUSB.println(" \n");
    menu_display_6 = true;
  }
}

void mode_7_airplane()
{
  if(menu_display_7)
  {
    SerialUSB.println("**Airplane Mode**");
    SerialUSB.println("This allows you to completely ignore logical I2C communication from the badge.");
    SerialUSB.println("In some cases, such as with DaBomb Badge, a peripheral (LED Driver) may share the same address 0x50.");
    SerialUSB.println("We realize Airplane Mode refers to wireless comms and is a stupid way to refer to this mode.");
    SerialUSB.println("But you all know what it means...");
    SerialUSB.println("If you plan to use this SAO with a badge that uses the same address we reccomend:");
    SerialUSB.println("Enable Persistence -> Enable Airplane Mode -> Disable Persistence\n");
    if(m7_airplane) SerialUSB.println("Airplane Mode is currently: ENABLED\n");
    else SerialUSB.println("Airplane Mode is currently: DISABLED\n");;

    SerialUSB.println("Press Q : Quit Back to the Main Menu");  
    SerialUSB.println("Press 1 : DISABLE Airplane Mode (default)"); 
    SerialUSB.println("Press 2 : ENABLE Airplane Mode"); 
    SerialUSB.print("Please enter a selection: "); 
    menu_display_7 = false; //this prevents infinite printing of the menu in loop   
  }

  // Press (Q or q) to quit back to main menu
  incomingByte = SerialUSB.read();
  if((incomingByte == 'q') || (incomingByte == 'Q'))
  {
    sao_mode = 0;
    menu_display_0 = true;
    menu_display_7 = true;
  }

  // m7_airplane mode Selection
  if((incomingByte == '1') || (incomingByte == '2')){
    if(incomingByte == '1'){//Disable Persisance
      if(m7_airplane)
      {
        // Only change it if it's different, so here its currently TRUE and they want it FALSE
        data.m7_airplane = false;
        persist_eeprom();
        m7_airplane = false;
      }
    }
    else
    {
      // Enable m7_airplane
      // Only change it if it's different, so here its currently FALSE and they want it TRUE
      if(!m7_airplane){
        data.m7_airplane = true;
        persist_eeprom();
        m7_airplane = true;
      }
    }
    // echo user selection to the USB terminal
    SerialUSB.print((char)incomingByte); 
    SerialUSB.println(" \n");
    menu_display_7 = true;
  }
}

void menu()
{
  // The purpose of the menu function is to externally call the sub-menu displays
  // Then within each case statement the processing logic for that menu is contained
  switch(sao_mode)
  {
    case 0:
      // Start message which displays in loop
      if(menu_display_start)
      {
        SerialUSB.println(" ");
        SerialUSB.print("Press X to Start: ");
      }

      // Main menu display
      if(menu_display_0)
      {
        macro_splash();
        SerialUSB.println("1 - DOOM Guy   Interface Mode");
        SerialUSB.println("2 - DOOM Guy Bus Sniffer Mode");
        SerialUSB.println("3 - I2C SAO  Bus Sniffer Mode");
        SerialUSB.println("4 - Serial UART  Sniffer Mode");
        SerialUSB.println("5 - Custom  Application  Mode");
        SerialUSB.println("6 - EEPROM  Persistence  Mode");
        SerialUSB.println("7 - Badge SAO  Airplane  Mode");
        SerialUSB.print("Please Select a Mode 1, 2, 3, 4, 5, 6, 7: ");  
        // this prevents infinite printing of the menu in loop   
        menu_display_0 = false;
      }

      incomingByte = SerialUSB.read();

      // Parse input for 'X' or 'x' to kickoff the main menu
      if(((incomingByte == 'x') || (incomingByte == 'X')) && (menu_display_0 == false))
      {
        menu_display_start = false;
        // echo user selection to the USB terminal
        SerialUSB.print((char)incomingByte);
        SerialUSB.println(" \n");
        menu_display_0 = true;
      }

      // If valid input 1..5 put the SAO in to that mode
      if((incomingByte >= '1') && (incomingByte <= '7') && (menu_display_start == false))
      {
        // echo user selection to the USB terminal
        SerialUSB.print((char)incomingByte); 
        SerialUSB.println(" \n");
        // Byte recieved is in ASCII, subtract 48 to get to the decimal value
        sao_mode = incomingByte - 48; 
        // empty the buffer
        while(Serial.available() > 0) incomingByte = Serial.read(); 
        //We want the face draws during interactive menu (option 1)
        if(sao_mode > 1) 
        {
          sao_god_mode_display = true; 
        }
        
        // This prevents the i2c msgs from displaying before the submenu splash
        if(sao_mode == 2)
        {
          mode_2_dg_sniffer(); 
        }
      }
      break;

    // Mode 1 - DOOM Guy EEPROM Interface
    case 1: mode_1_dg_interface(); break; 

    // Mode 2 - DOOM Guy Bus Sniffer
    case 2: mode_2_dg_sniffer(); break;
      
    // Mode 3 - I2C Sniffer
    case 3: mode_3_i2c_sniffer(); break;
      
    // Mode 4 - Serial Bus Sniffer
    case 4: mode_4_serial_sniffer(); break;
      
    // Mode 5 - Custom Application Mode
    case 5: mode_5_custom_appliaction(); break;

    // Mode 6 - EEPROM Persistence Mode
    case 6: mode_6_persistence(); break;

    // Mode 7 - Airplane Mode
    case 7: mode_7_airplane(); break;
  }
}

void setup(void)
{  
  // Join i2c bus with address #50 to simulate AT24C32 eeprom
  Wire.begin(0x50);                
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);
  
  SerialUSB.begin(9600);

  pinMode(GPIO1, INPUT);
  pinMode(GPIO2, INPUT);
  digitalWrite(GPIO1, LOW);
  digitalWrite(GPIO2, LOW);
  
  // Init ST7789 240x240
  tft.init(240, 240);             
  tft.setRotation(2);

  tft.fillScreen(ST77XX_WHITE);

  // That sweet MacroFab Sponsorship
  render(MF_Logo, sizeof(MF_Logo)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
  delay(500);
  tft.fillScreen(ST77XX_WHITE);

  // Load stored data
  data = sao_flash_store.read();
  // First time the SAO is ever ran the storage must be initialized
  if(!data.init)
  { 
    data.eeprom_3 = eeprom[3];
    data.eeprom_4 = eeprom[4];
    data.eeprom_5 = eeprom[5];
    data.m6_persistence = false;
    data.m7_airplane = false;
    data.init = true;
    m6_persistence_init_test = true;
    m7_airplane = false;
    persist_eeprom();
    // This will only display one time, post flash
    SerialUSB.println("Saving first time initialization data to flash");
  }
  // Read the persisted storage in to the virtual EEPROM
  else
  { 
    eeprom[3] = data.eeprom_3;
    eeprom[4] = data.eeprom_4;
    eeprom[5] = data.eeprom_5;
    m6_persistence = data.m6_persistence;
    m7_airplane = data.m7_airplane;
  }
}

void loop()
{
  // Default mode - DG Face Mode
  // Assumption is this mode is normally just "plugged into a badge"
  // If Auto mode: DG looks around 
  // If Badge Control mode: DG looks around depending on the state of GPIO 1/2
  // Depending on the heath value of the EEPROM DG will be more bloody!
  // Depending on the anger value of the EEPROM DG might be well....
  if(sao_mode < 2)
  {
    menu();
    // Automode ENABLED!
    if(eeprom[3] == 1)
    {
      automode_cycle++;
      if(automode_cycle > 3)
      {
        automode_cycle = 0;
      }
      cycle = automode_cycle;
    }
    // Automode Disabled! ASSUMING DIRECT BADGE CONTROL!
    else
    {
      // Look Forward
      if (digitalRead(GPIO1) == 0 && digitalRead(GPIO2) == 0)
      {
        cycle = 1;
      }
      // Look Left
      else if (digitalRead(GPIO1) == 1 && digitalRead(GPIO2) == 0)
      {
        cycle = 0;
      }
      // Look Right
      else if (digitalRead(GPIO1) == 0 && digitalRead(GPIO2) == 1)
      {
        cycle = 2;
      }
      // Whut?!
      else
      {
        cycle = 3;
      }
    }
    run_sao_mode_0(cycle);
    delay(500);
  }
  
  // It is better to disable the animation and the delays so they have real time control of the MCU.
  // Appropriately just display God Mode in this case.
  // There is a boolean toggle because it wastes time to continually redraw the same image
  else
  {  
    if(sao_god_mode_display) 
    {
      render(dg_god_mode, sizeof(dg_god_mode)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      sao_god_mode_display = false;
    }
    menu();
  }
}

void run_sao_mode_0(uint8_t face_dir)
{
  // DG Happy!
  if(eeprom[5] == 0)
  {
    if(m1_inconspicuous_variable == 1)
    {
      if(face_dir == 0)
      {
        render(bender_FullHP_left, sizeof(bender_FullHP_left)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
      }
      else if(face_dir == 1)
      {
        render(bender_FullHP_middle, sizeof(bender_FullHP_middle)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
      }
      else if(face_dir == 2)
      {
        render(bender_FullHP_right, sizeof(bender_FullHP_right)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
      }
      else
      {
        render(bender_FullHP_middle, sizeof(bender_FullHP_middle)/2, mf_offset_x, mf_offset_y, mf_pixel_size, mf_rez_x);
      }
    }
    else if(m1_lowkey_variable == 1)
    {
      render(dg_dealwithit, sizeof(dg_dealwithit)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
    }
    // DG Health 61
    else if(eeprom[4] == 0x3D)
    {
      if(face_dir == 0)
      {
        render(duke_FullHP_left, sizeof(duke_FullHP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(duke_FullHP_middle, sizeof(duke_FullHP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(duke_FullHP_right, sizeof(duke_FullHP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(eeprom[3] == 0)
      {
        render(duke_FullHP_cigar, sizeof(duke_FullHP_cigar)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(duke_FullHP_middle, sizeof(duke_FullHP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    else if(eeprom[4] >= 0x50)
    {
      if(face_dir == 0)
      {
        render(dg_FullHP_left, sizeof(dg_FullHP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_FullHP_middle, sizeof(dg_FullHP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_FullHP_right, sizeof(dg_FullHP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(eeprom[3] == 0)
      {
        render(dg_bald, sizeof(dg_bald)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_FullHP_middle, sizeof(dg_FullHP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG Health 60 or higher
    else if(eeprom[4] >= 0x3C)
    {
      if(face_dir == 0)
      {
        render(dg_80HP_left, sizeof(dg_80HP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {          
        render(dg_80HP_middle, sizeof(dg_80HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_80HP_right, sizeof(dg_80HP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_80HP_middle, sizeof(dg_80HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG Health 40 or higher
    else if(eeprom[4] >= 0x28)
    {    
      if(face_dir == 0)
      {      
        render(dg_60HP_left, sizeof(dg_60HP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_60HP_middle, sizeof(dg_60HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_60HP_right, sizeof(dg_60HP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_60HP_middle, sizeof(dg_60HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG Health 20 or higher
    else if(eeprom[4] >= 0x14)
    {
      if(face_dir == 0)
      {
        render(dg_40HP_left, sizeof(dg_40HP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_40HP_middle, sizeof(dg_40HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_40HP_right, sizeof(dg_40HP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_40HP_middle, sizeof(dg_40HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG Health 1 or higher     
    else if(eeprom[4] > 0x0)
    {
      if(face_dir == 0)
      {
        render(dg_20HP_left, sizeof(dg_20HP_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_20HP_middle, sizeof(dg_20HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_20HP_right, sizeof(dg_20HP_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_20HP_middle, sizeof(dg_20HP_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG DEAD. HEALTH = 0
    else
    {
      render(dg_dead, sizeof(dg_dead)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);         
    }
  }
  // DG ANGRY!
  else
  {
    // DG Anger 80 or higher
    if(eeprom[5] >= 0x50)
    {
      if(face_dir == 0)
      {
        render(dg_FullHP_angry_left, sizeof(dg_FullHP_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_FullHP_angry_middle, sizeof(dg_FullHP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_FullHP_angry_right, sizeof(dg_FullHP_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_FullHP_angry_middle, sizeof(dg_FullHP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
//    // DG Anger 60 or higher
//    else if(eeprom[5] >= 0x3C)
//    {
//      if(face_dir == 0)
//      {
//        render(dg_80HP_angry_left, sizeof(dg_80HP_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else if(face_dir == 1)
//      {
//        render(dg_80HP_angry_middle, sizeof(dg_80HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else if(face_dir == 2)
//      {
//        render(dg_80HP_angry_right, sizeof(dg_80HP_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else
//      {
//        render(dg_80HP_angry_middle, sizeof(dg_80HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//    }
    // DG Anger 40 or higher
    else if(eeprom[5] >= 0x28)
    {        
      if(face_dir == 0)
      {  
        render(dg_60HP_angry_left, sizeof(dg_60HP_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      } 
      else if(face_dir == 1)
      {
        render(dg_60HP_angry_middle, sizeof(dg_60HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_60HP_angry_right, sizeof(dg_60HP_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_60HP_angry_middle, sizeof(dg_60HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
    // DG Anger 20 or higher
    else if(eeprom[5] >= 0x14)
    {
      if(face_dir == 0)
      {
        render(dg_40HP_angry_left, sizeof(dg_40HP_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 1)
      {
        render(dg_40HP_angry_middle, sizeof(dg_40HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else if(face_dir == 2)
      {
        render(dg_40HP_angry_right, sizeof(dg_40HP_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
      else
      {
        render(dg_40HP_angry_middle, sizeof(dg_40HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
      }
    }
//    // DG Anger 1 or higher     
//    else if(eeprom[5] > 0x00)
//    {
//      if(face_dir == 0)
//      {
//        render(dg_20HP_angry_left, sizeof(dg_20HP_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else if(face_dir == 1)
//      {
//        render(dg_20HP_angry_middle, sizeof(dg_20HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else if(face_dir == 2)
//      {
//        render(dg_20HP_angry_right, sizeof(dg_20HP_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//      else
//      {
//        render(dg_20HP_angry_middle, sizeof(dg_20HP_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
//      }
//    }    
  }
  return;
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
  long current_write_time = millis();
 
  if((sao_mode == 1)||(sao_mode == 2)){
    SerialUSB.print("Write ");
    SerialUSB.print(value,HEX);
    SerialUSB.print(" hex to location ");
    SerialUSB.println(address, HEX);  
  }

  // Make sure address is not in the protected area!
  if(address >= 0x02)
  {
    // Writes to flash only persist if enabled AND it's different
    if(m6_persistence)
    {
      if((current_write_time - 30000) > previous_write_time)
      {
        if((address == 3) && (value != eeprom[address]))
        {
          data.eeprom_3 = value;  
          persist_eeprom();
          SerialUSB.println("Persistence Enabled - Saving Update to Flash...");
        } 
        else if((address == 3) && (value == eeprom[address]))
        {
          SerialUSB.println("Persistence Enabled - Not Updating Flash - Value Hasn't Changed!");
        }
        
        else if((address == 4) && (value != eeprom[address]))
        {
          data.eeprom_4 = value;
          persist_eeprom();  
          SerialUSB.println("Persistence Enabled - Saving Update to Flash...");
        }
        else if((address == 4) && (value == eeprom[address]))
        {
          SerialUSB.println("Persistence Enabled - Not Updating Flash - Value Hasn't Changed!");
        } 
           
        else if((address == 5) && (value != eeprom[address]))
        {
          data.eeprom_5 = value;
          persist_eeprom();  
          SerialUSB.println("Persistence Enabled - Saving Update to Flash...");
        }
        else if((address == 5) && (value == eeprom[address]))
        {
          SerialUSB.println("Persistence Enabled - Not Updating Flash - Value Hasn't Changed!");
        }
        // Update our write time
        previous_write_time = current_write_time;  
      }    
      else
      {
        SerialUSB.println("Persistence Enabled - Not Updating Flash - Timeout not Reached");
        SerialUSB.print("It has only been ");
        SerialUSB.print(current_write_time - previous_write_time);
        SerialUSB.println("ms since last write.");
      }
    }
    eeprom[address] = value;

    if(address == 0x13 && value == 0x37)
    {
      m1_lowkey_variable = 1;
    }
    else if(address == 0x13 && value == 0x00)
    {
      m1_lowkey_variable = 0;
    }
    
    if(address == 0x22 && value == 0x01)
    {
      m1_inconspicuous_variable = 1;
    }
    else if(address == 0x22 && value == 0x00)
    {
      m1_inconspicuous_variable = 0;
    }
  }
  else
  {
    if((sao_mode == 1)||(sao_mode == 2)){
      SerialUSB.print("Woah can't overwrite address: ");
      SerialUSB.println(address);
    }
  }

return;
}

void receiveEvent(int howMany)
{
  if(!m7_airplane){
    if(sao_mode == 2){
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
      if(sao_mode == 2){
        SerialUSB.println("No more bytes. Skip!");
      }
    }
    // DOOM SAO does not support multi byte reads and writes
    else
    {
      if(sao_mode == 2){
        SerialUSB.println("To many bytes! Not Supported!");
      }
    }
  }
}

void requestEvent()
{   
  if(!m7_airplane){
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
      if(sao_mode == 2){
        SerialUSB.print("requestEvent sending: 0x");
        SerialUSB.println(eeprom[mem_write_address], HEX);      
      }
      Wire.write(eeprom[mem_write_address]);
    }
    // Location doesn't exist. Return 0x00.
    else
    {
      if(sao_mode == 2){
        SerialUSB.print("Woah that is not a real address: ");
        SerialUSB.println(mem_write_address);
      }
      Wire.write(0x00);
    }
    return;
  }
}

void printHexByte(int b)
{
  SerialUSB.print((b >> 4) & 0xF, HEX);
  SerialUSB.print(b & 0xF, HEX);
}

int acquire_data()
{
  unsigned long endtime;
  unsigned long data,lastData;

  SerialUSB.println("Acquiring data");

  bool start = false;
  while (!start) 
  {
    while ((lastData = SAMPLE) != START1);          // wait until state is START1
    while ((data = SAMPLE) == lastData);            // wait until state change
    start = (data == START2);                       // start condtion met if new state is START2
  }

  endtime = millis() + TIMEOUT;
  lastData = START2;
  int k = 0;
  buffer[k++] = START1;
  buffer[k++] = START2;
  do 
  {
    while ((data = SAMPLE) == lastData);           // wait until data has changed
    buffer[k++] = lastData = data;
  }
  while ((k < BUFFSIZE) && (millis() < endtime));

  for(int i = 0; i < BUFFSIZE; i++)
  {
    buffer[i] = ((buffer[i] & PORT_PA22) >> 15) | ((buffer[i] & PORT_PA23) >> 17);
  }
  return k;
}

int gbuffer(int i)
{
    return (buffer[i] >> 6) & 0x3;
}

int findNextStartCondition(int k)
{
  while((k < BUFFSIZE- 1) && ((gbuffer(k - 1) != 3) || (gbuffer(k) != 1)))
    k++;
  return k;
}

void display_data(int points)
{
  int lastData, data, k, Bit, Byte, i, state,nextStart;
  long starttime;

  #define ADDRESS  0     

  starttime = millis();

  SerialUSB.print("Analyzing data, number of transitions = "); SerialUSB.println(points);
  SerialUSB.println("I2C response activity: * = NAK . = ACK");
  SerialUSB.println("Everything is in HEX");
  // ignore start transition
  k = 3;              
  i = 0;
  Byte = 0;
  state = ADDRESS;
  nextStart = findNextStartCondition(k);
  do {
    data = gbuffer(k++);
    if (data & 1) {
      Bit = (data & 2) > 0;
      Byte = (Byte << 1) + Bit;
      if (i++ >= 8) {
        if (state == ADDRESS) {
          SerialUSB.print("Dev=");
          printHexByte(Byte / 4);
          if (Byte & 2)
            SerialUSB.print(" R");
          else
            SerialUSB.print(" W");
          if (Byte & 1)
            SerialUSB.print("*");
          else
            SerialUSB.print(".");
          state++;
        }       
        else if (state == 1) {
          SerialUSB.print("Data=");
          printHexByte(Byte / 2);
          if (Byte & 1)
            SerialUSB.print("*");
          else
            SerialUSB.print(".");
          state++;
          //k--;                      // why ??? <- this was the original comment for this line. This caused the data to be read off one byte. IE 0x16 would be read as 0x0B. Hint found here. https://github.com/rricharz/i2c-sniffer-100kBaud-Arduino-Mega/issues/3
        }
        else {
          printHexByte(Byte / 2);
          if (Byte & 1)
            SerialUSB.print("*");
          else
            SerialUSB.print(".");
        }
        if (nextStart - k < 9) {
          SerialUSB.println();
          k = nextStart + 1;
          nextStart = findNextStartCondition(k);
          state = ADDRESS;
        }
        i = 0;
        Byte = 0;
      }
    }
  }
  while (k < points);
  SerialUSB.println("");
  SerialUSB.print("Time to analyze and display data was ");
  SerialUSB.print(millis() - starttime);
  SerialUSB.println(" msec");
  SerialUSB.println();
}
