#ifndef GAMEEXCEPTION_H
#define GAMEEXCEPTION_H

#include <exception>

#include <QString>

#include "gameobject.h"


class GameException : public std::exception {

    public:
        enum Cause {
            UnknownGameObjectType = 0,
            CouldNotOpenGameObjectFile,
            CorruptGameObjectFile,
            InvalidGameObjectFileName,
            InvalidGameObjectPointer,
            NullPointerReference,
            InvalidGameObjectCast,
            InvalidFunctionCode,
            InvalidCharacterStats,
            InvalidCombatMessage,
            InvalidPoint,
            InvalidVector,
            UnknownGameEventType,
            NotSupported,
            IndexOutOfBounds,
            NullIteratorReference
        };

        GameException(Cause cause);
        GameException(Cause cause, GameObjectType objectType, uint id);
        GameException(Cause cause, const QString &message);
        virtual ~GameException() throw ();

        Cause cause() const { return m_cause; }

        virtual const char *what() const throw ();

    private:
        static const char *s_messages[];

        Cause m_cause;

        char *m_what;
};

#endif // GAMEEXCEPTION_H
