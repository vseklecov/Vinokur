#ifndef __PUBLISHER_H__
#define __PUBLISHER_H__

#include <Arduino.h>

union Event {
    char value_ch;
    int value_int;
    unsigned char value_byte;
    float value_float;
    void *value_ptr;
};

class Publisher;

class Subscriber
{
  public:
    Subscriber() {}
    virtual bool recive(Event ev, Publisher *pub) = 0;

  private:
};

class Publisher
{
  public:
    Publisher() : stackSubs(NULL) {}
    ~Publisher();
    virtual void setup(void) = 0;
    virtual void service(void) = 0;

    bool subscribe(Subscriber*);
    Subscriber *unsubscribe(void);

    void publish(Event ev);

  private:
    struct Stack
    {
        Subscriber *sub;
        Stack *next;
    } * stackSubs;
};

#endif