/**************************************************************************/
/**
 * \brief EEPROM 24C01 /02/04/08/16/32/64/128/254/512/1024 library for Arduino - Demonstration program
 * \author Copyright (C) 2018  Pierre Valleau - www.zoubworld.com
 * \version 2.0
 * \date 201804224
 * Based on the work of  Julien Le Sech - www.idreammicro.com( Copyright (C) 2012 )
 * source : https://github.com/jlesech/Eeprom24C01_02.git 
 * Extended to 04/08/16/32/64/128/254/512/1024 Eeprom
 *
 * This file is part of the EEPROM 24C01 /02/04/08/16/32/64/128/254/512/1024 library for Arduino.
 *
 * This library is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 ******************************************************************************/


/**************************************************************************//**
 * \file WriteReadByte.ino
 ******************************************************************************/
 
/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include <Wire.h>

#include <ZEeprom.h>

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \def EEPROM_ADDRESS
 * \brief Address of EEPROM memory on TWI bus.
 ******************************************************************************/
#define EEPROM_ADDRESS  0x50

/******************************************************************************
 * Private variable definitions.
 ******************************************************************************/

 ZEeprom * eeprom;

/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

 const byte address = 0;

 byte data = 0;
 byte outputBytes[4] = { 0 };

/**************************************************************************//**
 * \fn void setup()
 *
 * \brief
 ******************************************************************************/
void setup()
{
    // Initialize serial communication.
    Serial.begin(9600);
        
    // Initialize EEPROM library.
     eeprom= new ZEeprom();
    eeprom->begin(Wire,AT24Cxx_BASE_ADDR,AT24C02);
}

/**************************************************************************//**
 * \fn void loop()
 *
 * \brief
 ******************************************************************************/
void loop()
{
    eeprom->writeByte(0x03, 0xBA);
  
//    for (byte i = 0; i < 4; i++)
//    {
//      data = eeprom->readByte(i);
//      
//      Serial.print("Read byte = 0x");
//      Serial.print(data, HEX);
//      Serial.println("");
//    }

    delay(2000);
}
