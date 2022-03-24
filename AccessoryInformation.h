/*
 * Creates all the boilerplate information for an accessory.
 *
 * An accessory may also be identifiable either via a connected LED
 * or through the internal LED if defined.
 */
#ifndef ACCESSORY_INFORMATION_H
#define ACCESSORY_INFORMATION_H

#include <HomeSpan.h>
#include "Config.h"

const char* accManufacturer = ACCESSORY_MANUFACTURER;
const char* accSerialNumber = ACCESSORY_SERIAL_NUMBER;
const char* accVersion = ACCESSORY_VERSION;

struct AccessoryInformationNonIdentifiable : Service::AccessoryInformation {
  
  AccessoryInformationNonIdentifiable(
    const char *name,
    const char *manu = accManufacturer,
    const char *sn = accSerialNumber,
    const char *model = "kalle",
    const char *version = accVersion
  ) : Service::AccessoryInformation(){
    new Characteristic::Name(name);
    new Characteristic::Manufacturer(manu);
    new Characteristic::SerialNumber(sn);    
    new Characteristic::Model(model);
    new Characteristic::FirmwareRevision(version);
    new Characteristic::Identify();
  }
};

#endif 