#include "gameexception.h"

#include <cstdio>
#include <cstring>

#include "gameobject.h"


const char *GameException::s_messages[] = {
    "Unknown game object type",
    "Cannot open game object file",
    "Corrupt game object file",
    "Invalid game object file name",
    "Invalid game object pointer",
    "Null pointer referenced",
    "Game object pointer casted to invalid type",
    "Invalid function code (Parse Error)",
    "Invalid character stats",
    "Invalid combat message",
    "Invalid point",
    "Invalid vector",
    "Not supported"
};

GameException::GameException(Cause cause) :
    std::exception(),
    m_cause(cause),
    m_customMessage(false),
    m_message(const_cast<char *>(s_messages[cause])) {
}

GameException::GameException(Cause cause, GameObjectType objectType, uint id) :
    std::exception(),
    m_cause(cause),
    m_customMessage(true) {

    m_message = new char[100];
    if (objectType != GameObjectType::Unknown) {
        sprintf(m_message, "%s (Object: %s:%d)", s_messages[cause],
                objectType.toCString(), id);
    } else {
        sprintf(m_message, "%s (Object: undefined:%d)", s_messages[cause], id);
    }
}

GameException::GameException(GameException::Cause cause, const char *message) :
    std::exception(),
    m_cause(cause),
    m_customMessage(true) {

    m_message = new char[100 + strlen(message)];
    sprintf(m_message, "%s: %s", s_messages[cause], message);
}

GameException::~GameException() throw () {

    if (m_customMessage) {
        delete[] m_message;
    }
}

const char *GameException::what() const throw () {

    return m_message;
}
