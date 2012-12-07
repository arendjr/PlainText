#include "gameexception.h"

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
    "Unknown game event type",
    "Not supported",
    "Index out of bounds",
    "Null iterator referenced",
    "Invalid sign-in"
};

GameException::GameException(Cause cause) :
    std::exception(),
    m_cause(cause),
    m_what(strdup(s_messages[cause])) {
}

GameException::GameException(Cause cause, GameObjectType objectType, uint id) :
    std::exception(),
    m_cause(cause) {

    m_what = strdup(QString("%1 (%2:%3)").arg(QString(s_messages[cause]), objectType.toString(),
                                              QString::number(id)).toUtf8().constData());
}

GameException::GameException(GameException::Cause cause, const QString &message) :
    std::exception(),
    m_cause(cause) {

    m_what = strdup(QString("%1: %2").arg(QString(s_messages[cause]), message)
                    .toUtf8().constData());
}

GameException::~GameException() throw () {

    delete[] m_what;
}

const char *GameException::what() const throw () {

    return m_what;
}
