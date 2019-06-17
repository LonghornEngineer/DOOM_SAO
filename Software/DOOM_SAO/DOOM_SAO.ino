#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
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

uint16_t time = 0;

uint8_t mem_address = 0;

uint8_t eeprom[4] = {
  0x1B, 0x05, 0x01, 0x41
};

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup(void)
{
  Wire.begin(0x50);                // join i2c bus with address #4
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);
  
  SerialUSB.begin(9600);

  tft.init(240, 240);           // Init ST7789 240x240
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


void render(const uint16_t dg_face[], int16_t siz, int16_t offset_x, int16_t offset_y, int16_t pixel_size, int16_t rez_x)
{
  int16_t x_loc = 0;
  int16_t y_loc = 0;

  int16_t x = 0;
  int16_t y = 0;

  for(int i = 0; i < siz; i++)
  {    
    x = (x_loc * pixel_size) + offset_x; 
    y = (y_loc * pixel_size) + offset_y;
    tft.fillRect(x, y, pixel_size, pixel_size, dg_face[x_loc + (y_loc * rez_x)]);
  
    x_loc++;
    if(x_loc == rez_x)
    {
      x_loc = 0;
      y_loc++;
    }
  }

  return;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  SerialUSB.println("receiveEvent");
  SerialUSB.print("How Many Bytes: ");
  SerialUSB.println(howMany);

  uint8_t receive_buffer[howMany] = { 0 };
  
  while(1 < Wire.available()) // loop through all but the last
  {
    SerialUSB.println("Wire Available");
    char c = Wire.read(); // receive byte as a character
    SerialUSB.println(byte(c));         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  mem_address = x;
  SerialUSB.println(byte(x));         // print the integer
}

void requestEvent()
{
    SerialUSB.println("requestEvent");
    //char c = Wire.read();
    //SerialUSB.println(byte(c));
    Wire.write(eeprom[mem_address]);
    return;
}
