#include "session.h"

#include <QCryptographicHash>
#include <QDebug>

#include "engine/area.h"
#include "engine/character.h"
#include "engine/commandinterpreter.h"
#include "engine/realm.h"


class Session::SignUpData {

    public:
        QString userName;
        QString password;
};

Session::Session(QObject *parent) :
    QObject(parent),
    m_signInStage(SessionClosed),
    m_signUpData(0),
    m_character(0) {
}

Session::~Session() {

    if (m_character) {
        if (m_interpreter) {
            m_character->leave(m_character->currentArea());
        }
        m_character->setSession(0);
    }
    delete m_signUpData;
}

void Session::open() {

    write("What is your name? ");
    m_signInStage = AskingUserName;
}

void Session::onUserInput(QString data) {

    if (m_signInStage == SessionClosed) {
        qDebug() << "User input on closed session";
        return;
    }

    if (!m_character || !m_character->isAdmin()) {
        data = data.left(160);
    }

    if (m_signInStage != SignedIn) {
        processSignIn(data);
        return;
    }

    Q_ASSERT(m_interpreter);
    m_interpreter->execute(data);
}

void Session::processSignIn(const QString &data) {

    QString input = data.trimmed();

    QString passwordHash;
    switch (m_signInStage) {
        case AskingUserName:
            m_character = Realm::instance()->getCharacter(input);
            if (m_character) {
                m_signInStage = AskingPassword;
            } else {
                m_signUpData = new SignUpData();
                m_signUpData->userName = input;
                m_signInStage = AskingUserNameConfirmation;
            }
            break;

        case AskingUserNameConfirmation:
            if (input.toLower() == "yes" || input.toLower() == "correct") {
                m_signInStage = AskingSignupPassword;
            } else {
                m_signInStage = SignInAborted;
            }
            break;

        case AskingPassword:
            passwordHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toBase64();
            if (m_character->passwordHash() == passwordHash) {
                write(QString("Welcome back, %1").arg(m_character->name()));
                m_signInStage = SignedIn;
            }  else {
                write("Password incorrect.");
            }
            break;

        case AskingSignupPassword:
            m_signUpData->password = input;
            m_signInStage = AskingSignupPasswordConfirmation;
            break;

        case AskingSignupPasswordConfirmation:
            if (m_signUpData->password == input) {
                passwordHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toBase64();
                m_character = qobject_cast<Character *>(GameObject::createByObjectType("character"));
                Q_ASSERT(m_character);

                m_character->setName(m_signUpData->userName);
                m_character->setPasswordHash(passwordHash);
                m_character->setCurrentArea(Realm::instance()->getObject("area", 1));

                delete m_signUpData;
                m_signUpData = 0;
                m_signInStage = SignedIn;
            } else {
                write("Passwords don't match.");
                m_signInStage = AskingSignupPassword;
            }
            break;

        default:
            break;
    }

    switch (m_signInStage) {
        case AskingUserNameConfirmation:
            write(QString("%1, did I get that right? ").arg(m_signUpData->userName));
            break;

        case AskingPassword:
            write("Please enter your password: ");
            break;

        case AskingSignupPassword:
            write("Please choose a password: ");
            break;

        case AskingSignupPasswordConfirmation:
            write("Confirm your password: ");
            break;

        case SignedIn:
            if (m_character->session() != 0) {
                write("Cannot sign you in because you're already signed in from another location.");
                terminate();
                break;
            }
            m_character->setSession(this);
            connect(m_character, SIGNAL(write(QString)), this, SIGNAL(write(QString)));

            m_interpreter = new CommandInterpreter(m_character, this);
            connect(m_interpreter, SIGNAL(quit()), this, SIGNAL(terminate()));
            m_character->enter(m_character->currentArea());
            break;

        case SignInAborted:
            write("Ok. Bye.");
            terminate();
            break;

        default:
            break;
    }
}
