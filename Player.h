/*
 * A service that represents a "player", aka. a set of keys, that
 * may be "playing" as one "Player N" .
 * 
 * The service builds the player as an occupancy sensor linked to
 * four controller slots (represented as outlets). This means that
 * when a key is inserted or removed the sensor will trigger that
 * the player has joined or left and the corresponding outlet will
 * turn on or off. The outlet used will be somewhat hidden in the
 * HomeApp but will be available for automations.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <HomeSpan.h>
#include "Config.h"
#include "AccessoryInformation.h"
#include "QueueMaster.h"
#include "ControllerPort.h"
#include "Subscriber.h"

#define NUMBER_OF_CONTROLLER_PORTS 4

struct Player : Service::OccupancySensor, Subscriber {
  SpanCharacteristic* isPlaying;
  const char* playerName;
  int playerNbr;

  ControllerPort* controllerPorts[NUMBER_OF_CONTROLLER_PORTS];
  int playerIsOnControllerPort = -1;

	Player(const char* playerName, int playerNbr) :
    Service::OccupancySensor(),
    Subscriber()
  {
    this->playerName = playerName;
    this->playerNbr = playerNbr;
    
		this->isPlaying = new Characteristic::OccupancyDetected();
    const char* isPlayingSuffix = " is playing";
    char* isPlayingName = new char[strlen(playerName) + strlen(isPlayingSuffix) + 1];
    strcpy(isPlayingName, playerName);
    strcat(isPlayingName, isPlayingSuffix);

    new Characteristic::Name(isPlayingName);


    const char* controllerPortNames[NUMBER_OF_CONTROLLER_PORTS] = DEFAULT_CONTROLLER_PORT_NAMES;
    
    for (int i = 0; i < NUMBER_OF_CONTROLLER_PORTS; i++) {
      // NOTE: we don't create AccessoryInformation here, but HomeApp seems to pick it up from the OccupancySensor
      const char* portName = controllerPortNames[i];
      LOG1("Creating controller port: "); LOG1(portName); LOG1("\n");
      // new SpanAccessory();
      // new AccessoryInformationNonIdentifiable(portName, "Nontendo","123-ABC","FourScore","1.0");
      ControllerPort* cP = new ControllerPort(portName);
      // addLink(cP); // ignore intellisense warning. compiles and works.
      controllerPorts[i] = cP;
    }
	}

  void onNewSubscribedValue(int newPlayerNbr, int onPortNbr) {
    // player subscribes to updates from controller port which means that (int newValue, int originId)
    // from Subscriber is (int newPlayerNbr, int onPortNbr)
    if (newPlayerNbr == this->playerNbr) {
      this->playerIsOnControllerPort = onPortNbr;
      updateHomeKitStatus(onPortNbr);
    } else if (onPortNbr == this->playerIsOnControllerPort) {
      // player used to be on this port, but is no longer
      this->playerIsOnControllerPort = -1;
      updateHomeKitStatus(-1);    
    }
  }

  void updateHomeKitStatus (int onPort) {
    LOG1("Player ["); LOG1(playerName);
    if (onPort > -1) {
      LOG1("] is playing on port "); LOG1(onPort);
      this->isPlaying->setVal(1);
    } else {
      LOG1("] is not playing");
      this->isPlaying->setVal(0);
    }
    LOG1("\n");

    LOG2("Updating controller statuses."); LOG2("\n");
    for (int i = 0; i < NUMBER_OF_CONTROLLER_PORTS; i++) {
      ControllerPort* cP = controllerPorts[i]; // could also use vector<SpanService *> getLinks(), but not clear which is prefered.
      if (i == onPort) {
        cP->turnOn();
      } else {
        cP->turnOff();
      }
    }
  }
};

void playerFactory(QueueMaster* qm) {
  const int nbrOfPlayers = NUMBER_OF_PLAYERS;
  const char* playerNames[nbrOfPlayers] = DEFAULT_PLAYER_NAMES;
  const int readGpioPins[4] = ANALOGUE_READ_GPIO_PINS;

  Queuer* measuringControllerPorts[NUMBER_OF_CONTROLLER_PORTS];

  int gpioPinForMeasuring = readGpioPins[0];

  for (int i = 0; i < NUMBER_OF_CONTROLLER_PORTS; i++) {
    measuringControllerPorts[i] = new MeasuringControllerPort(gpioPinForMeasuring, i);
    qm->addQueuer(measuringControllerPorts[i], i, false);
  }

  LOG2("Creating "); LOG2(nbrOfPlayers); LOG2(" players on pin "); LOG2(gpioPinForMeasuring); LOG2("\n");

  for (int i = 0; i < nbrOfPlayers; i++) {
      new SpanAccessory();
        new AccessoryInformationNonIdentifiable(playerNames[i]);

        LOG1("Giving player number (corresponding to controller pin) "); LOG1(i); LOG1("\n");
        LOG1("Naming player "); LOG1(playerNames[i]); LOG1("\n");
        Player* p = new Player(playerNames[i], i);
        for (int i = 0; i < NUMBER_OF_CONTROLLER_PORTS; i++) {
          measuringControllerPorts[i]->subscribe(p);
        }
  }
}

#endif