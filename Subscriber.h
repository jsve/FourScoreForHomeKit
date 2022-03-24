/*
 * A class that extends this can register as subscriber to a Queuer
 * The subscriber will not have to implement it's own loop function,
 * but will instead get its onNewSubscribedValue executed when needed.
 *
 * onNewSubscribedValue needs to be overloaded for the type the Queuer
 * passes as parameter to the method
 */
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

struct Subscriber {

  Subscriber() {}

  virtual void onNewSubscribedValue(int newValue, int originId) = 0;
};

#endif