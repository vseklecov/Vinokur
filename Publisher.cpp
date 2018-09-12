#include <Arduino.h>

#include "Publisher.h"

Publisher::~Publisher()
{
    while (stackSubs != NULL)
        unsubscribe();
}

bool Publisher::subscribe(Subscriber *sub)
{
    Publisher::Stack *cur = new Publisher::Stack();
    if (cur == NULL)
        return false;
    cur->sub = sub;
    cur->next = stackSubs;
    stackSubs = cur;
    return true;
}

Subscriber *Publisher::unsubscribe()
{
    Subscriber *sub = NULL;

    if (stackSubs != NULL)
    {
        Publisher::Stack *cur = stackSubs;
        stackSubs = cur->next;
        sub = cur->sub;
        free(cur);
    };

    return sub;
}

void Publisher::publish(Event ev)
{
    for (Publisher::Stack *cur = stackSubs; cur != NULL; cur = cur->next)
        if (cur->sub->recive(ev, this))
            break;
}
