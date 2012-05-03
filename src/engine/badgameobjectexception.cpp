#include "badgameobjectexception.h"


const char *BadGameObjectException::s_messages[] = {
    "Unknown game object type",
    "Cannot open game object file",
    "Corrupt game object file",
    "Invalid game object file name",
    "Invalid game object pointer",
    "Invalid exit format"
};

BadGameObjectException::BadGameObjectException(Cause cause):
    std::exception(),
    m_cause(cause) {
}

BadGameObjectException::~BadGameObjectException() throw (){
}

const char *BadGameObjectException::what() const throw () {

    return s_messages[m_cause];
}
