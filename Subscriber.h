/*
 * A class that extends this can register as subscriber to a Queuer
 * The subscriber will not have to implement it's own loop function,
 * but will instead get its onNewSubscribedValue executed when needed.
 *
 * The value sent to onNewSubscribedValue depends on which type of Queuer
 * the subscriber is subscribed to. The subscriber needs to keep track of
 * what the values mean themselves.
 */
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

struct Subscriber {

  Subscriber() {}

  virtual void onNewSubscribedValue(int newValue, int originId) = 0;
};

#endif