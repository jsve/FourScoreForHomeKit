/*
 * Anyone who implements Queuer can be part of the queue handled by the QueueMaster
 * The Queuer has one or more tasks that can be carried out when it is
 * their turn in the queue. This is defined in the loop(); and is called by the
 * QueueMaster when it's the Queuers turn.
 *
 * Extending classes must implement it's own tasks in loop(); These should not
 * be executed if finishedWithTasks is true, which is set by the QueueMaster through
 * reset() since there potentially can be two queuers on the same channel, and we don't
 * want the first one to finish to restart its loop() before the second one has finished.
 */
#ifndef QUEUER_H
#define QUEUER_H

#define MAX_NBR_OF_SUBSCRIBERS 6

#include <HomeSpan.h>

#include "Subscriber.h"

struct QueueMaster;

struct Queuer {
  Subscriber* subscribers[MAX_NBR_OF_SUBSCRIBERS];
  int nbrOfSubscribers = 0;

  int id;

  int lastPublishedValue = -1;

  bool finishedWithTasks = false;

  Queuer(int id = 0) {
    this->id = id;
  }

  virtual void loop() = 0;
  
  virtual bool shouldPublishToSubscribers(int newValue) = 0;

  virtual int preprocessValueBeforePublish(int newValue) {
    return newValue;
  }

  void onTasksDone(int newValue) {
    if (this->shouldPublishToSubscribers(newValue)) {
      for (int i = 0; i < nbrOfSubscribers; i++) {
        subscribers[i]->onNewSubscribedValue(preprocessValueBeforePublish(newValue), id);
      };
      this->lastPublishedValue = newValue;
    }

    finishedWithTasks = true;
  }

  void reset() {
    finishedWithTasks = false;
  }

  void subscribe(Subscriber* s) {
    if (nbrOfSubscribers < MAX_NBR_OF_SUBSCRIBERS) {
      subscribers[nbrOfSubscribers++] = s;
    } else {
      LOG1("WARNING: Tried to subscribe to full subscriber list"); LOG1("\n");
    }
  }
};



#endif