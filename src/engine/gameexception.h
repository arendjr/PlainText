#ifndef GAMEEXCEPTION_H
#define GAMEEXCEPTION_H

#include <exception>

#include <Qt>


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
            NotSupported
        };

        GameException(Cause cause);
        GameException(Cause cause, const char *objectType, uint id);
        GameException(Cause cause, const char *message);
        virtual ~GameException() throw ();

        Cause cause() const { return m_cause; }

        virtual const char *what() const throw ();

    private:
        static const char *s_messages[];

        Cause m_cause;

        bool m_customMessage;
        char *m_message;
};

#endif // GAMEEXCEPTION_H
