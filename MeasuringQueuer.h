/*
 * Measures some value (averaged) and updates that to HomeKit. Each device implements its own
 * updateHomeKitStatus and interpretGpioReading however meningful to them.
 */
#ifndef MEASURING_QUEUER_H
#define MEASURING_QUEUER_H

#include <HomeSpan.h>
#include "QueueMaster.h"
#include "Queuer.h"
#include "Config.h"

const int nbrOfReadingsForAverage = NUMBER_OF_READINGS_FOR_AVERAGE;

struct MeasuringQueuer : Queuer {
  int gpioPin;
  int readingVariance;
  int interferenceReading;

  // for averaging
  bool isMeasuringForAverage = false;
  int readings[nbrOfReadingsForAverage] = {0};
  int readIndex = 0;
  int totalOfCurrentReadings = 0;
  
  // default 0 if queuer won't have subscribers or they don't care about which queuer sent the update
	MeasuringQueuer(int gpioPin, int id = 0) :
    Queuer(id)
  {
    this->gpioPin = gpioPin;
    this->readingVariance = READING_VARIANCE;
    this->interferenceReading = INTERFERENCE_READING;

    // initialize all the readings for averaging to 0:
    // for (int thisReading = 0; thisReading < nbrOfReadingsForAverage; thisReading++) {
    //   readings[thisReading] = 0;
    // }
	}

  int getAveragePinReading() {
    int ar = analogRead(this->gpioPin);

    this->readings[this->readIndex] = ar;
    this->totalOfCurrentReadings += ar;
    this->readIndex ++;

    if (this->readIndex >= nbrOfReadingsForAverage) {
      this->readIndex = 0;

      int average = this->totalOfCurrentReadings / nbrOfReadingsForAverage;
      this->totalOfCurrentReadings = 0; // after calculating average, this can also be reset.
      
      return average;
    }
    return -1; // averaging still processing
  }

  bool shouldPublishToSubscribers(int newValue) {
    if (
        this->lastPublishedValue > newValue - readingVariance &&
        this->lastPublishedValue < newValue + readingVariance
      ) {
        // NOTE: this might be a good place to stabilize irradic results. i.e. if same value is measured twice (or N times) - update
        // to HomeKit. In that case, we probably don't want to save the last calculated average,
        // but instead the value from the config-arrays.

        // port not changed. waiting...
        return false;
      }
      return true;
  }

  void loop() {
    if(this->finishedWithTasks) {
      return;
    };

    /* the value returned by analogRead() will fluctuate based on a number
      * of factors (e.g. the values of the other analog inputs, how close
      * your hand is to the board, etc.). Averiging is nessesary
      */
    int measuredAverage = getAveragePinReading();
    if (measuredAverage > -1) {
      LOG2("raw average measured by device on pin "); LOG2(this->gpioPin); LOG2(": "); LOG2(measuredAverage); LOG2("\n");
      this->onTasksDone(measuredAverage);
    }
    // else: stillmeasuring
  }
};

namespace measurementHelpers {
  int getPositionInArrayWithVariance(int val, int var, const int arr[], int arrLength) {
    for (int i = 0; i < arrLength; i++) {
      if (
        val > arr[i] - var &&
        val < arr[i] + var
      ) {
        return i;
      }
    }
    return -1;
  }
}

#endif