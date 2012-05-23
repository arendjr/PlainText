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
            InvalidFunctionCode,
            InvalidExitFormat
        };

        GameException(Cause cause);
        GameException(Cause cause, const char *objectType, uint id);
        virtual ~GameException() throw ();

        virtual const char *what() const throw ();

    private:
        static const char *s_messages[];

        bool m_customMessage;
        char *m_message;
};

#endif // GAMEEXCEPTION_H
