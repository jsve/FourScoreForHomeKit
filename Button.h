/*
 * A service that represents two buttons. Buttons are created in
 * pairs to be functional with the queueMaster and its two reader
 * positions.
 *
 * The service builds the button as an outlet, with a secondary outlet
 * created by the first. The primary outlet is responsible for looping
 * and updating the status of the secondary button.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <HomeSpan.h>
#include "Config.h"
#include "AccessoryInformation.h"
#include "QueueMaster.h"
#include "MeasuringQueuer.h"
#include "StatefulSwitch.h"
#include "Subscriber.h"

struct Button : MeasuringQueuer, Subscriber {
  const int gpioReadings[6] = GPIO_READINGS;
  const int nbrOfButtonStates = 2;

  Button(int gpioPin) : MeasuringQueuer(gpioPin), Subscriber() {
    this->subscribe(this);
  }

  // MeasuringQueuer methods:
  int preprocessValueBeforePublish(int reading) override {
    if (reading <= interferenceReading) {
      return -1;
    }
    const int buttonStates[2] = {gpioReadings[0], gpioReadings[1]};

    int buttonState = measurementHelpers::getPositionInArrayWithVariance(
      reading,
      readingVariance,
      buttonStates,
      nbrOfButtonStates
    );

    return buttonState;
  }

  // SubscriberMethods
    void onNewSubscribedValue(int buttonState, int _) override {
    updateHomeKitStatus(buttonState);
  }

  virtual void updateHomeKitStatus(int buttonState) = 0;
};

struct TurboButton : StatefulSwitch, Button {
	TurboButton(int gpioPin, const char* buttonName) : StatefulSwitch(buttonName), Button(gpioPin) {}

  void updateHomeKitStatus (int buttonState) override{
    LOG1("Turbo button "); LOG1(name); LOG1(" ");
    if (buttonState == -1) {
      LOG1("is in no state?!"); LOG1("\n");
      this->turnOff();
      return;
    }
    if (buttonState == 0) {
      LOG1("is in no turbo state"); LOG1("\n");
      this->turnOff();
      return;
    }
    if (buttonState == 1) {
      LOG1("is in turbo state"); LOG1("\n");
      this->turnOn();
      return;
    }
  }
};

struct NbrOfPlayersSwitch : StatefulSwitch, Button {
  StatefulSwitch* secondaryButton;

	NbrOfPlayersSwitch(int gpioPin, const char* buttonName, const char* secondaryButtonName) :
    StatefulSwitch(buttonName),
    Button(gpioPin)
  { 
    new SpanAccessory();
      new AccessoryInformationNonIdentifiable(secondaryButtonName, "Nontendo","123-ABC","FourScore","1.0");
      this->secondaryButton = new StatefulSwitch(secondaryButtonName);
	}  

  void updateHomeKitStatus (int buttonState) override {
    LOG1("Two / four player switch ");
    if (buttonState == -1) {
      LOG1("is in no state?!"); LOG1("\n");
      this->turnOff();
      secondaryButton->turnOff();
      return;
    }
    if (buttonState == 0) {
      LOG1("is in four player state (primary)"); LOG1("\n");
      this->turnOn();
      secondaryButton->turnOff();
      return;
    }
    if (buttonState == 1) {
      LOG1("is in two player state (secondary)"); LOG1("\n");
      this->turnOff();
      secondaryButton->turnOn();
      return;
    }
  }
};

void buttonFactory(QueueMaster* qm) {
  const char* buttonNames[4] = {"Four Player Mode", "Two Player Mode", "Turbo A", "Turbo B"};
  const int readGpioPins[4] = ANALOGUE_READ_GPIO_PINS;

  int gpioPin = readGpioPins[1]; // we read buttons on other pin than players.
  LOG2("Creating buttons on pin "); LOG2(gpioPin); LOG2("\n");
  
  for (int i = 0; i < 4; i++) { // NOTE: 4 even though two/four player gets created as one button
    if (i == 1) continue; // fourPlayerMode
    LOG1("Creating "); LOG1(buttonNames[i]); LOG1(" index: "); LOG1(i); LOG1("\n");
    Queuer* button;
    if (i > 0) { // i.e. not two/four player mode switch
      new SpanAccessory();
        new AccessoryInformationNonIdentifiable(buttonNames[i]);
        button = new TurboButton(
          gpioPin,
          buttonNames[i]
        );
    } else {
    new SpanAccessory();
      new AccessoryInformationNonIdentifiable(buttonNames[i]);
      button = new NbrOfPlayersSwitch(
        gpioPin,
        buttonNames[i],
        buttonNames[i+1]
      );
    }
    qm->addQueuer(button, i, true);
  }
}

#endif