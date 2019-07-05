#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include "DG_Faces.h"
#include "MF_Logo.h"

#define TFT_CS        -1
#define TFT_RST        9
#define TFT_DC         8

int16_t dg_offset_x = 16;
int16_t dg_offset_y = 0;
int16_t dg_pixel_size = 8;
int16_t dg_rez_x = 26;

int16_t mf_offset_x = 0;
int16_t mf_offset_y = 0;
int16_t mf_pixel_size = 4;
int16_t mf_rez_x = 60;

uint8_t mem_write_address = 0x00;

uint8_t eeprom[5] = {
  0x1B, 0x05, 0x01, 0x41, 0x00
};

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

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
  render(dg_normal_left, sizeof(dg_normal_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_normal_middle, sizeof(dg_normal_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);
 
  render(dg_normal_right, sizeof(dg_normal_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500); 
  
  render(dg_normal_middle, sizeof(dg_normal_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_angry_left, sizeof(dg_angry_left)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_angry_middle, sizeof(dg_angry_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_angry_right, sizeof(dg_angry_right)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_damage_middle, sizeof(dg_damage_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_evil_middle, sizeof(dg_evil_middle)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);

  render(dg_god_mode, sizeof(dg_god_mode)/2, dg_offset_x, dg_offset_y, dg_pixel_size, dg_rez_x);
  delay(500);
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
  SerialUSB.print("Write ");
  SerialUSB.print(value,HEX);
  SerialUSB.print(" to location ");
  SerialUSB.println(address, HEX);

  // Make sure address is not in the protected area!
  if(address >= 0x04)
  {
    eeprom[address] = value;
  }
  else
  {
    SerialUSB.print("Woah can't overwrite address: ");
    SerialUSB.println(address);
  }
  return;
}

void receiveEvent(int howMany)
{
  SerialUSB.print("receiveEvent bytes received: ");
  SerialUSB.println(howMany);

  // 1 Byte Received. This is a Read From EEPROM Operation. 
  if(howMany == 1)
  {
    uint8_t x = Wire.read();                 // receive byte
    mem_write_address = x;
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
    SerialUSB.println("No more bytes. Skip!");
  }
  // DOOM SAO does not support multi byte reads and writes
  else
  {
    SerialUSB.println("To many bytes! Not Supported!");
  }
}

void requestEvent()
{    
    // Check to see if the location exists!
    if(mem_write_address < sizeof(eeprom))
    {
      SerialUSB.print("requestEvent sending: 0x");
      SerialUSB.println(eeprom[mem_write_address], HEX);
      Wire.write(eeprom[mem_write_address]);
    }
    // Location doesn't exist. Return 0x00.
    else
    {
      SerialUSB.print("Woah that is not a real address: ");
      SerialUSB.println(mem_write_address);
      Wire.write(0x00);
    }

    mem_write_address = 0x00;
    return;
}
