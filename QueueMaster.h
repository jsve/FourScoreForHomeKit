/*
 * To allow proper read of different controller ports and buttons,
 * each channel needs to be measured seperately. This is done through a
 * multiplexer that controlled with 2 outputs, giving 2x4 channels.
 *
 * First 1x4 channels are 1YN on the multiplexer and second 1x4 channels
 * are 2YN. These are called queues below. This means there are 2 parallell
 * queues with 4 channels each.
 */

#ifndef QUEUEMASTER_H
#define QUEUEMASTER_H

#include <HomeSpan.h>
#include "Config.h"
#include "Queuer.h"

struct QueueMaster {
  int nbrOfChannels = 1; // will get increased during initiation.
  uint32_t timer=millis();

  int outputGpioPins[2];
  int sampleInterval;
  
  int currentChannel = -1;
  int lastCompletedChannel = -1;
  int maxNbrOfChannels = 4; // same as nbr of rows in arrays below
  Queuer* registeredQueuers[4][2] = {0}; // [maxNbrOfChannels][maxNbrOfQueues]

  QueueMaster(const int outputGpioPins[2], int sampleInterval) {
    this->outputGpioPins[0] = outputGpioPins[0];
    this->outputGpioPins[1] = outputGpioPins[1];
    this->sampleInterval = sampleInterval;

    setPinsForChannel(0);
  }

  void loop() {
    if (currentChannel > -1) {
      if (queuerIsBusy()) {
        runQueuerLoop();
        return;
      } else {
        LOG2("done with cycle "); LOG2(lastCompletedChannel);  LOG2("\n");
        prepareNextChannel();
        return;
      }
    }

    if(millis() - timer > (sampleInterval / nbrOfChannels)) {
      activateNextChannel();
    };
  }

  Queuer* getCurrentChannelQueuer(int i) {
    if (currentChannel > -1) {
      if (registeredQueuers[currentChannel][i] != 0) {
        return registeredQueuers[currentChannel][i];
      }
    }
    return 0;
  }

  bool queuerIsBusy() {
    for(int i = 0; i < 2; i++) {
      if (getCurrentChannelQueuer(i) == 0) continue;
      if (!getCurrentChannelQueuer(i)->finishedWithTasks) return true;
    }
    return false;
  }

  void runQueuerLoop() {
    for(int i = 0; i < 2; i++) {
      if (getCurrentChannelQueuer(i) == 0) continue;
      getCurrentChannelQueuer(i)->loop();
    }
  }

  void resetQueuers() {
    for(int i = 0; i < 2; i++) {
      if (getCurrentChannelQueuer(i) == 0) continue;
      // LOG2("Resetting for "); LOG2(currentChannel); LOG2(":"); LOG2(i); LOG2("\n");
      getCurrentChannelQueuer(i)->reset();
    }
  }

  void addQueuer(Queuer* queuer, int channel, bool useSecondaryQueue) {
    LOG2("Adding queuer to channel "); LOG2(channel); LOG2(" in secondary "); LOG2(useSecondaryQueue); LOG2("\n");
    const int queueNbr = useSecondaryQueue ? 1 : 0;
    const int suggestedNbrOfChannels = channel + 1;
    
    if (suggestedNbrOfChannels > this->nbrOfChannels) {
      this->nbrOfChannels = suggestedNbrOfChannels;
    }
    registeredQueuers[channel][queueNbr] = queuer;
  }

  Queuer* getAnyQueuer(int channel, bool useSecondaryQueue) {
    const int queueNbr = useSecondaryQueue ? 1 : 0;

    if (channel > maxNbrOfChannels || channel < 0) {
      LOG1("WARNING: channel out of allowed range. Channel: "); LOG1(channel); LOG1(" max number of channels: "); LOG1(maxNbrOfChannels); LOG1("\n");
      return 0;
    }
    Queuer* q = registeredQueuers[channel][queueNbr];
    if (q == 0) {
      LOG1("WARNING: there is no queuer for this channel: "); LOG1(channel); LOG1(" and queue number: "); LOG1(queueNbr); LOG1("\n");
      return 0;
    }
    return q;
  }

  int nextChannelNbr() {
    if (lastCompletedChannel >= this->nbrOfChannels - 1) {
      return 0;
    }
    return (lastCompletedChannel + 1);
  }

  void prepareNextChannel() {
    lastCompletedChannel = currentChannel;
    resetQueuers();

    currentChannel = -1;
    const int channelToPrepare = nextChannelNbr();
    LOG2("preparing for channel "); LOG2(channelToPrepare);  LOG2("\n");
    setPinsForChannel(channelToPrepare);

    timer = millis(); // reset timer to cause next cycle to start after a delay relative to this.
  }

  void activateNextChannel() {
    if (currentChannel >= 0) {
      LOG1("WARNING: last channel was not read properly or not at all. faking last channel read"); LOG1("\n");
      prepareNextChannel();
      // NOTE: we want to continue as nothing happened after preparing.
    }
  
    currentChannel = nextChannelNbr();

    // skip cycle if there are no readers for that channel (shouldn't happen):
    if (getCurrentChannelQueuer(0) == 0 && getCurrentChannelQueuer(1) == 0) {
      LOG1("WARNING: No registered readers for next cycle. skipping..."); LOG1("\n");
      prepareNextChannel();
      activateNextChannel();
      return;
    }
  }

  void setPinsForChannel(int channel) {
    // NOTE: should be binary, i.e. 0,0 0,1 1,0 1,1 but something fishy is going on that is left as debt.
    if (channel == -1) { // binary 0
      digitalWrite(outputGpioPins[0], LOW);
      digitalWrite(outputGpioPins[1], LOW);
    }
    if (channel == 0) { // binary 1
      digitalWrite(outputGpioPins[0], HIGH);
      digitalWrite(outputGpioPins[1], HIGH);
    }
    if (channel == 1) { // binary 2
      digitalWrite(outputGpioPins[0], HIGH);
      digitalWrite(outputGpioPins[1], LOW);
    }
    if (channel == 2) { // binary 3
      digitalWrite(outputGpioPins[0], LOW);
      digitalWrite(outputGpioPins[1], HIGH);
    }
    if (channel == 3) { // binary 4
      digitalWrite(outputGpioPins[0], LOW);
      digitalWrite(outputGpioPins[1], LOW);
    }
  }
    
};

#endif