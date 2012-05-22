#include "session.h"

#include <QCryptographicHash>
#include <QDebug>

#include "engine/area.h"
#include "engine/commandinterpreter.h"
#include "engine/player.h"
#include "engine/realm.h"
#include "engine/util.h"


class Session::SignUpData {

    public:
        QString userName;
        QString password;
};

Session::Session(QObject *parent) :
    QObject(parent),
    m_signInStage(SessionClosed),
    m_signUpData(0),
    m_player(0) {
}

Session::~Session() {

    if (m_player) {
        if (m_interpreter) {
            m_player->leave(m_player->currentArea());
        }
        m_player->setSession(0);
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

    if (!m_player || !m_player->isAdmin()) {
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
        case AskingUserName: {
            QString userName = Util::capitalize(input);
            m_player = Realm::instance()->getPlayer(userName);
            if (m_player) {
                m_signInStage = AskingPassword;
            } else {
                m_signUpData = new SignUpData();
                m_signUpData->userName = userName;
                m_signInStage = AskingUserNameConfirmation;
            }
            break;
        }
        case AskingUserNameConfirmation:
            if (input.toLower() == "yes" || input.toLower() == "correct") {
                m_signInStage = AskingSignupPassword;
            } else {
                m_signInStage = SignInAborted;
            }
            break;

        case AskingPassword:
            passwordHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toBase64();
            if (m_player->passwordHash() == passwordHash) {
                write(QString("Welcome back, %1. Type %2 if you're feeling lost.\n")
                      .arg(m_player->name(), Util::colorize("help", White)));
                m_signInStage = SignedIn;
            }  else {
                write("Password incorrect.\n");
            }
            break;

        case AskingSignupPassword:
            m_signUpData->password = input;
            m_signInStage = AskingSignupPasswordConfirmation;
            break;

        case AskingSignupPasswordConfirmation:
            if (m_signUpData->password == input) {
                passwordHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toBase64();
                m_player = qobject_cast<Player *>(GameObject::createByObjectType("player"));
                Q_ASSERT(m_player);

                m_player->setName(m_signUpData->userName);
                m_player->setPasswordHash(passwordHash);
                m_player->setCurrentArea(Realm::instance()->getObject("area", 1));

                delete m_signUpData;
                m_signUpData = 0;
                m_signInStage = SignedIn;
            } else {
                write("Passwords don't match.\n");
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
            if (m_player->session() != 0) {
                write("Cannot sign you in because you're already signed in from another location.");
                terminate();
                break;
            }
            m_player->setSession(this);
            connect(m_player, SIGNAL(write(QString)), this, SIGNAL(write(QString)));

            m_interpreter = new CommandInterpreter(m_player, this);
            connect(m_interpreter, SIGNAL(quit()), this, SIGNAL(terminate()));
            m_player->enter(m_player->currentArea());
            break;

        case SignInAborted:
            write("Ok. Bye.");
            terminate();
            break;

        default:
            break;
    }
}
