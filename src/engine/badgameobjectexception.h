#ifndef BADGAMEOBJECTEXCEPTION_H
#define BADGAMEOBJECTEXCEPTION_H

#include <exception>


class BadGameObjectException : public std::exception {

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

        BadGameObjectException(Cause cause);
        virtual ~BadGameObjectException() throw ();

        virtual const char *what() const throw ();

    private:
        static const char *s_messages[];

        Cause m_cause;
};

#endif // BADGAMEOBJECTEXCEPTION_H
