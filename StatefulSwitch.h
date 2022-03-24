/*
 * A wrapper around the HomeSpan outlet, used to represent a stateful switch.
 * 
 * A stateful switch is a physical switch that is not meant to be controlled from
 * HomeKit, but only read and used for automations.
 */

#ifndef STATEFUL_SWITCH_H
#define STATEFUL_SWITCH_H

#include <HomeSpan.h>

struct StatefulSwitch : Service::Outlet {
  SpanCharacteristic* isOn;
  SpanCharacteristic* isUsed;
  
  const char* name;

  StatefulSwitch(const char* name) : Service::Outlet() {
    /* removePerms can be used to not allow value to be changed through apps. note that
     * this causes HomeApp and other apps to register the device as faulty, and a restart
     * of the app might be needed to remove the rendered (!) on the device icon. It still works though.
     *
     * Another way to do this is to override the update method and disregard the new value there (see below).
     *
     * The update hack is used now, but removePerms might be prefered in the future if the apps respect this properly.
     */
    
    // isOn = (new Characteristic::On())->removePerms(PW);
    // isUsed = (new Characteristic::OutletInUse())->removePerms(PW);
    
    isOn = new Characteristic::On();
    isUsed = new Characteristic::OutletInUse();
    new Characteristic::Name(name);


    this->name = name;

  }

  void turnOn() {
    // LOG2("turning on "); LOG2(this->name); LOG2("\n");
    if (!isOn->getVal()) isOn->setVal(true);
    if (!isUsed->getVal()) isUsed->setVal(true);
  }

  void turnOff() {
    // LOG2("turning off "); LOG2(this->name); LOG2("\n");
    if (isOn->getVal()) isOn->setVal(false);
    if (isUsed->getVal()) isUsed->setVal(false);
  }

  boolean update() override {
    isOn->setVal(isOn->getVal());
    isUsed->setVal(isUsed->getVal());
    return(true);
  }
};

#endif