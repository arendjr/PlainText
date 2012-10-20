#include "session.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QMap>

#include "room.h"
#include "characterstats.h"
#include "class.h"
#include "commandevent.h"
#include "commandinterpreter.h"
#include "constants.h"
#include "gameobjectptr.h"
#include "logutil.h"
#include "player.h"
#include "race.h"
#include "realm.h"
#include "signinevent.h"
#include "util.h"


struct Session::SignUpData {

    QString userName;
    QString password;

    QString gender;
};

Session::Session(Realm *realm, const QString &source, QObject *parent) :
    QObject(parent),
    m_source(source),
    m_signInStage(SessionClosed),
    m_signUpData(nullptr),
    m_realm(realm),
    m_player(nullptr) {

    LogUtil::logSessionEvent(m_source, "Session opened");
}

Session::~Session() {

    LogUtil::logSessionEvent(m_source, "Session closed");

    if (m_player && m_player->session() == this) {
        LogUtil::logCommand(m_player->name(), "(signed out)");

        m_player->setSession(nullptr);
    }
    delete m_signUpData;
}

void Session::open() {

    setSignInStage(AskingUserName);
    askForUserName();
}

void Session::processSignIn(const QString &data) {

    if (m_signInStage == SignedIn) {
        CommandEvent event(m_player, data);
        event.process();
        return;
    } else if (m_signInStage == SessionClosed) {
        return;
    }

    QString input = data.trimmed();
    QString answer = input.toLower();

    switch (m_signInStage) {
        case AskingUserName:
            processUserName(answer);
            break;

        case AskingUserNameConfirmation:
            processUserNameConfirmation(answer);
            break;

        case AskingPassword:
            processPassword(input);
            break;

        case AskingSignupPassword:
            processSignupPassword(input);
            break;

        case AskingSignupPasswordConfirmation:
            processSignupPasswordConfirmation(input);
            break;

        case AskingGender:
            processGender(answer);
            break;

        case AskingSignupConfirmation:
            processSignupConfirmation(answer);
            break;

        default:
            break;
    }

    switch (m_signInStage) {
        case AskingUserName:
            askForUserName();
            break;

        case AskingUserNameConfirmation:
            askForUserNameConfirmation();
            break;

        case AskingPassword:
            askForPassword();
            break;

        case AskingSignupPassword:
            askForSignupPassword();
            break;

        case AskingSignupPasswordConfirmation:
            askForSignupPasswordConfirmation();
            break;

        case AskingGender:
            askForGender();
            break;

        case AskingSignupConfirmation:
            askForSignupConfirmation();
            break;

        case SignedIn:
            m_player->setSession(this);
            connect(m_player, SIGNAL(write(QString)), this, SIGNAL(write(QString)));

            m_player->enter(m_player->currentRoom());
            break;

        case SignInAborted:
            write("Ok. Bye.");
            emit terminate();
            break;

        default:
            break;
    }
}

void Session::signOut() {

    setSignInStage(SessionClosed);
    emit terminate();
}

void Session::onUserInput(QString data) {

    if (m_signInStage == SessionClosed || m_signInStage == SignInAborted) {
        qDebug() << "User input on closed session";
        return;
    }

    if (data.isEmpty()) {
        return;
    }

    if (!m_player || !m_player->isAdmin()) {
        data = data.left(160);
    }

    if (m_signInStage == SignedIn) {
        m_realm->enqueueEvent(new CommandEvent(m_player, data));
    } else {
        m_realm->enqueueEvent(new SignInEvent(this, data));
    }
}

void Session::setSignInStage(Session::SignInStage signInStage) {

    if (m_signInStage == signInStage) {
        return;
    }

    m_signInStage = signInStage;

    switch (m_signInStage) {
        case AskingUserName:
            startAskingUserName();
            break;

        case AskingUserNameConfirmation:
            startAskingUserNameConfirmation();
            break;

        case AskingPassword:
            startAskingPassword();
            break;

        case AskingSignupPassword:
            startAskingSignupPassword();
            break;

        case AskingSignupPasswordConfirmation:
            startAskingSignupPasswordConfirmation();
            break;

        case AskingGender:
            startAskingGender();
            break;

        case AskingSignupConfirmation:
            startAskingSignupConfirmation();
            break;

        default:
            break;
    }
}

void Session::startAskingUserName() {
}

void Session::askForUserName() {

    write("What is your name? ");
}

void Session::processUserName(const QString &answer) {

    QString userName = Util::validateUserName(answer);
    if (userName.length() < 3) {
        if (!userName.isEmpty()) {
            write("I'm sorry, but your name should consist of at least 3 letters.\n");
        }
        return;
    }

    m_player = m_realm->getPlayer(userName);
    if (m_player) {
        setSignInStage(AskingPassword);
    } else if (m_realm->reservedNames().contains(userName)) {
        write("Yeah right, like I believe that...\n");
    } else {
        m_signUpData = new SignUpData();
        m_signUpData->userName = userName;
        setSignInStage(AskingUserNameConfirmation);
    }
}

void Session::startAskingUserNameConfirmation() {
}

void Session::askForUserNameConfirmation() {

    write(QString("%1, did I get that right? ").arg(m_signUpData->userName));
}

void Session::processUserNameConfirmation(const QString &answer) {

    if (answer == "yes" || answer == "y") {
        setSignInStage(AskingSignupPassword);
    } else if (answer == "no" || answer == "n") {
        setSignInStage(SignInAborted);
    } else {
        write("Please answer with yes or no.\n");
    }
}

void Session::startAskingPassword() {
}

void Session::askForPassword() {

    write("Please enter your password: ");
}

void Session::processPassword(const QString &input) {

    QByteArray data = QString(m_player->passwordSalt() + input).toUtf8();
    QString passwordHash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();
    if (m_player->passwordHash() == passwordHash) {
        LogUtil::logSessionEvent(m_source, "Authentication success for player " + m_player->name());
        LogUtil::logCommand(m_player->name(), "(signed in)");

        if (m_player->session()) {
            write("Cannot sign you in because you're already signed in from another location.\n");
            setSignInStage(SessionClosed);
            emit terminate();
            return;
        }

        write(QString("Welcome back, %1. Type %2 if you're feeling lost.\n").arg(m_player->name(),
              Util::highlight("help")));
        setSignInStage(SignedIn);
    }  else {
        LogUtil::logSessionEvent(m_source, "Authentication failed for player " + m_player->name());

        write("Password incorrect.\n");
    }
}

void Session::startAskingSignupPassword() {
}

void Session::askForSignupPassword() {

    write("Please choose a password: ");
}

void Session::processSignupPassword(const QString &input) {

    if (input.length() < 6) {
        write(Util::colorize("Please choose a password of at least 6 characters.\n", Red));
        return;
    }
    if (input.toLower() == m_signUpData->userName.toLower()) {
        write(Util::colorize("Your password and your username may not be the same.\n", Red));
        return;
    }
    if (input == "123456" || input == "654321") {
        write(Util::colorize("Sorry, that password is too simple.\n", Red));
        return;
    }
    m_signUpData->password = input;
    setSignInStage(AskingSignupPasswordConfirmation);
}

void Session::startAskingSignupPasswordConfirmation() {
}

void Session::askForSignupPasswordConfirmation() {

    write("Please confirm your password: ");
}

void Session::processSignupPasswordConfirmation(const QString &input) {

    if (m_signUpData->password == input) {
        write(Util::colorize("Password confirmed.\n", Green));
        setSignInStage(AskingGender);
    } else {
        write(Util::colorize("Passwords don't match.\n", Red));
        setSignInStage(AskingSignupPassword);
    }
}

void Session::startAskingGender() {

    write("\n"
          "Please select which gender you would like your character to be.\n"
          "Your gender has a minor influence on the physique of your character.");
}

void Session::askForGender() {

    write(QString("\n"
                  "Please choose %1 or %2.\n"
                  "\n")
          .arg(Util::highlight("male"),
               Util::highlight("female")));
}

void Session::processGender(const QString &answer) {

    if (answer == "male" || answer == "m") {
        write(Util::colorize("\nYou have chosen to be male.\n", Green));
        m_signUpData->gender = "male";
        setSignInStage(AskingSignupConfirmation);
    } else if (answer == "female" || answer == "f") {
        write(Util::colorize("\nYou have chosen to be female.\n", Green));
        m_signUpData->gender = "female";
        setSignInStage(AskingSignupConfirmation);
    }
}

void Session::startAskingSignupConfirmation() {

    write(QString("\n"
                  "You will become a %1 soldier.\n"
                  "\n")
          .arg(m_signUpData->gender));
}

void Session::askForSignupConfirmation() {

    write("\n"
          "Are you ready to create this character?\n");
}

void Session::processSignupConfirmation(const QString &answer) {

    if (answer == "yes" || answer == "y") {
        QString salt = Util::randomString(8);
        QByteArray data = QString(salt + m_signUpData->password).toUtf8();
        QString hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();

        Race *humanRace = qobject_cast<Race *>(m_realm->getObject("race", 2));
        Class *soldierClass = qobject_cast<Class *>(m_realm->getObject("class", 3));
        CharacterStats stats = humanRace->stats() + soldierClass->stats();
        int height = humanRace->height();
        int weight = humanRace->weight();
        if (m_signUpData->gender == "male") {
            stats.strength++;
            height += 10;
            weight += 15;
        } else {
            stats.dexterity++;
            weight -= 5;
        }

        CharacterStats randomStats;
        for (int i = 0; i < 9; i++) {
            int attr = qrand() % 6;
            if (attr == 0) {
                randomStats.strength++;
            } else if (attr == 1) {
                randomStats.dexterity++;
            } else if (attr == 2) {
                randomStats.vitality++;
            } else if (attr == 3) {
                randomStats.endurance++;
            } else if (attr == 4) {
                randomStats.intelligence++;
            } else {
                randomStats.faith++;
            }
        }
        stats += randomStats;
        height += randomStats.intelligence - (randomStats.dexterity / 2);
        weight += randomStats.strength;

        m_player = GameObject::createByObjectType<Player *>(m_realm, "player");

        m_player->setAdmin(m_realm->players().isEmpty());
        m_player->setName(m_signUpData->userName);
        m_player->setPasswordSalt(salt);
        m_player->setPasswordHash(hash);
        m_player->setRace(humanRace);
        m_player->setClass(soldierClass);
        m_player->setGender(m_signUpData->gender);
        m_player->setStats(stats);
        m_player->setHeight(height);
        m_player->setWeight(weight);
        m_player->setCurrentRoom(humanRace->startingArea());

        m_player->setHp(m_player->maxHp());
        m_player->setMp(m_player->maxMp());
        m_player->setGold(100);

        delete m_signUpData;
        m_signUpData = 0;

        LogUtil::logSessionEvent(m_source, "Character created for player " + m_player->name());
        LogUtil::logCommand(m_player->name(), "(signed in)");

        write(QString("\nWelcome to %1, %2.\n").arg(m_realm->name(), m_player->name()));
        setSignInStage(SignedIn);
    } else if (answer == "no" || answer == "n" ||
               answer == "back" || answer == "b") {
        setSignInStage(AskingGender);
    } else {
        write("Please answer with yes or no.\n");
    }
}

void Session::showColumns(const QStringList &items) {

    int length = items.length();
    int halfLength = length / 2 + length % 2;
    for (int i = 0; i < halfLength; i++) {
        QString first = Util::capitalize(items[i]);
        QString second = Util::capitalize(i + halfLength < length ? items[i + halfLength] : "");

        write("  " + Util::highlight(first.leftJustified(30)) +
              "  " + Util::highlight(second));
    }
}
