/*
 * A service that represents a controller port that
 * a player (aka a set of keys) may play on (be connected to).
 * 
 * The service builds the controller port as a stateful switch (outlet) which
 * can be linked to the player. Each player needs its own set of
 * four controller ports. The state of these four pseudo-ports is mapped from the
 * actual ports, defined as MeasuringControllerPort.
 *
 * The MeasuringControllerPort represents the physical port and is not exposed directly
 * to HomeKit. Its subscribers will be Players, which in turn have ControllerPorts
 * that are exposed. This means that there will be a 1:4 relationship between
 * MeasuringControllerPorts and ControllerPorts. This is to make things look nice in HomeKit.
 */

#ifndef CONTROLLER_PORT_H
#define CONTROLLER_PORT_H

#include <HomeSpan.h>
#include "StatefulSwitch.h"
#include "MeasuringQueuer.h"

struct ControllerPort : StatefulSwitch {
  ControllerPort(const char* portName) : StatefulSwitch(portName) {};
};

struct MeasuringControllerPort : MeasuringQueuer {
  const int gpioReadings[6] = GPIO_READINGS;
  const int nbrOfPlayers = NUMBER_OF_PLAYERS;
  
  MeasuringControllerPort(int gpioPin, int controllerPortNbr) :
    MeasuringQueuer(
      gpioPin,
      controllerPortNbr
    )
  {}

  int preprocessValueBeforePublish(int reading) override {
    if (reading <= interferenceReading) {
      return -1;
    }

    int pluggedInPlayer = measurementHelpers::getPositionInArrayWithVariance(reading, readingVariance, gpioReadings, nbrOfPlayers);

    return pluggedInPlayer;
  }
};

#endif