#ifndef EVENT_H
#define EVENT_H

#include <QString>


class Event {

    public:
        Event();
        virtual ~Event();

        virtual void process() = 0;

        virtual QString toString() const = 0;
};

#endif // EVENT_H
