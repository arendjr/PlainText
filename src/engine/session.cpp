#include "session.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QMap>

#include "area.h"
#include "characterstats.h"
#include "class.h"
#include "commandevent.h"
#include "commandinterpreter.h"
#include "constants.h"
#include "gameobjectptr.h"
#include "player.h"
#include "race.h"
#include "realm.h"
#include "signinevent.h"
#include "util.h"


struct Session::SignUpData {

    QString userName;
    QString password;

    QMap<QString, GameObjectPtr> races;
    QMap<QString, GameObjectPtr> classes;

    Race *race;
    Class *characterClass;
    QString gender;

    CharacterStats stats;
    int height;
    int weight;

    SignUpData() :
        race(nullptr),
        characterClass(nullptr),
        height(0),
        weight(0) {
    }
};

Session::Session(Realm *realm, QObject *parent) :
    QObject(parent),
    m_signInStage(SessionClosed),
    m_signUpData(nullptr),
    m_realm(realm),
    m_player(nullptr),
    m_interpreter(nullptr) {
}

Session::~Session() {

    if (m_player && m_player->session() == this) {
        m_player->setSession(nullptr);
    }
    delete m_signUpData;

    delete m_interpreter;
}

void Session::open() {

    setSignInStage(AskingUserName);
    askForUserName();
}

void Session::processSignIn(const QString &data) {

    if (m_signInStage == SignedIn) {
        CommandEvent event(m_interpreter, data);
        event.process();
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

        case AskingRace:
            processRace(answer);
            break;

        case AskingClass:
            processClass(answer);
            break;

        case AskingGender:
            processGender(answer);
            break;

        case AskingExtraStats:
            processExtraStats(answer);
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

        case AskingRace:
            askForRace();
            break;

        case AskingClass:
            askForClass();
            break;

        case AskingGender:
            askForGender();
            break;

        case AskingExtraStats:
            askForExtraStats();
            break;

        case AskingSignupConfirmation:
            askForSignupConfirmation();
            break;

        case SignedIn:
            if (m_player->session()) {
                write("Cannot sign you in because you're already signed in from another location.");
                terminate();
                break;
            }
            m_player->setSession(this);
            connect(m_player, SIGNAL(write(QString)), this, SIGNAL(write(QString)));

            m_interpreter = new CommandInterpreter(m_player);
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

void Session::onUserInput(QString data) {

    if (m_signInStage == SessionClosed) {
        qDebug() << "User input on closed session";
        return;
    }

    if (data.isEmpty()) {
        return;
    }

    if (!m_player || !m_player->isAdmin()) {
        data = data.left(160);
    }

    if (m_signInStage == SignedIn && m_interpreter) {
        m_realm->enqueueEvent(new CommandEvent(m_interpreter, data));
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

        case AskingRace:
            startAskingRace();
            break;

        case AskingClass:
            startAskingClass();
            break;

        case AskingGender:
            startAskingGender();
            break;

        case AskingExtraStats:
            startAskingExtraStats();
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
        write(QString("Welcome back, %1. Type %2 if you're feeling lost.\n").arg(m_player->name(),
              Util::highlight("help")));
        setSignInStage(SignedIn);
    }  else {
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
        write(Util::colorize("Please choose a password of at least 6 characters.\n", Maroon));
        return;
    }
    if (input.toLower() == m_signUpData->userName.toLower()) {
        write(Util::colorize("Your password and your username may not be the same.\n", Maroon));
        return;
    }
    if (input == "123456" || input == "654321") {
        write(Util::colorize("Sorry, that password is too simple.\n", Maroon));
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
        setSignInStage(AskingRace);
    } else {
        write(Util::colorize("Passwords don't match.\n", Maroon));
        setSignInStage(AskingSignupPassword);
    }
}

void Session::startAskingRace() {

    if (m_signUpData->races.isEmpty()) {
        for (auto racePtr : m_realm->races()) {
            if (racePtr.cast<Race *>()->playerSelectable()) {
                m_signUpData->races[racePtr->name()] = racePtr;
            }
        }
    }

    write("\n"
          "Please select which race you would like your character to be.\n"
          "Your race determines some attributes of the physique of your character, as well as "
          "where in the " GAME_TITLE " you will start your journey.\n"
          "\n"
          "These are the major races in the " GAME_TITLE ":\n"
          "\n");
    showColumns(m_signUpData->races.keys());
}

void Session::askForRace() {

    write(QString("\n"
                  "Please select the race you would like to use, or type %1 to get more "
                  "information about a race.\n")
          .arg(Util::highlight("info <race>")));
}

void Session::processRace(const QString &answer) {

    if (m_signUpData->races.keys().contains(answer)) {
        write(Util::colorize(QString("\nYou have chosen to become a %1.\n").arg(answer), Green));
        m_signUpData->race = m_signUpData->races[answer].cast<Race *>();
        setSignInStage(AskingClass);
    } else if (answer.startsWith("info ")) {
        QString raceName = answer.mid(5).trimmed();
        if (m_signUpData->races.keys().contains(raceName)) {
            write("\n" +
                  Util::highlight(Util::capitalize(raceName)) + "\n  " +
                  Util::splitLines(m_signUpData->races[raceName].cast<Race *>()->description(),
                                   78).join("\n  ") + "\n");
        } else if (raceName.startsWith("<") && raceName.endsWith(">")) {
            write(QString("Sorry, you are supposed to replace <race> with the name of an actual "
                          "race. For example: %1.\n").arg(Util::highlight("info human")));
        } else {
            write(QString("I don't know anything about the \"%1\" race.\n").arg(raceName));
        }
    }
}

void Session::startAskingClass() {

    for (auto classPtr : m_signUpData->race->classes()) {
        m_signUpData->classes[classPtr->name()] = classPtr;
    }

    write("\n"
          "Please select which class you would like your character to be specialized in.\n"
          "Your class determines additional attributes of the physique of your character, and "
          "also can influence your choice to be good or evil.\n");
    write(QString("\n"
                  "Note that the available classes are dependent on your choice of race. To "
                  "revisit your choice of race, type %1.\n")
          .arg(Util::highlight("back")));
    write("These are the classes you may choose from:\n"
          "\n");
    showColumns(m_signUpData->classes.keys());
}

void Session::askForClass() {

    write(QString("\n"
                  "Please select the class you would like to use, or type %1 to get more "
                  "information about a class.\n")
          .arg(Util::highlight("info <class>")));
}

void Session::processClass(const QString &answer) {

    if (m_signUpData->classes.keys().contains(answer)) {
        write(Util::colorize(QString("\nYou have chosen to become a %1.\n").arg(answer), Green));
        m_signUpData->characterClass = m_signUpData->classes[answer].cast<Class *>();
        setSignInStage(AskingGender);
    } else if (answer.startsWith("info ")) {
        QString className = answer.mid(5).trimmed();
        if (m_signUpData->classes.keys().contains(className)) {
            write("\n" +
                  Util::highlight(Util::capitalize(className)) + "\n  " +
                  Util::splitLines(m_signUpData->classes[className].cast<Class *>()->description(),
                                   78).join("\n  ") + "\n");
        } else if (className.startsWith("<") && className.endsWith(">")) {
            write(QString("Sorry, you are supposed to replace <class> with the name of an actual "
                          "race. For example: %1.\n").arg(Util::highlight("info knight")));
        } else {
            write(QString("I don't know anything about the \"%1\" class.\n").arg(className));
        }
    } else if (answer == "back" || answer == "b") {
        setSignInStage(AskingRace);
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
                  "\n"
                  "To revisit your choice of class, type %3.\n")
          .arg(Util::highlight("male"),
               Util::highlight("female"),
               Util::highlight("back")));
}

void Session::processGender(const QString &answer) {

    if (answer == "male" || answer == "m") {
        write(Util::colorize("\nYou have chosen to be male.\n", Green));
        m_signUpData->gender = "male";
        setSignInStage(AskingExtraStats);
    } else if (answer == "female" || answer == "f") {
        write(Util::colorize("\nYou have chosen to be female.\n", Green));
        m_signUpData->gender = "female";
        setSignInStage(AskingExtraStats);
    } else  if (answer == "back" || answer == "b") {
        setSignInStage(AskingClass);
    }
}

void Session::startAskingExtraStats() {

    m_signUpData->stats = m_signUpData->race->stats() + m_signUpData->characterClass->stats();
    m_signUpData->height = m_signUpData->race->height();
    m_signUpData->weight = m_signUpData->race->weight();

    if (m_signUpData->characterClass->name() == "knight") {
        m_signUpData->weight += 10;
    } else if (m_signUpData->characterClass->name() == "warrior" ||
               m_signUpData->characterClass->name() == "soldier") {
        m_signUpData->weight += 5;
    } else if (m_signUpData->characterClass->name() == "barbarian") {
        m_signUpData->stats.intelligence = 0;
        m_signUpData->weight += 5;
    }

    if (m_signUpData->gender == "male") {
        m_signUpData->stats.strength++;
        m_signUpData->height += 10;
        m_signUpData->weight += 10;
    } else {
        m_signUpData->stats.dexterity++;
        m_signUpData->weight -= 10;
    }

    write(QString("\n"
                  "You have selected to become a %1 %2 %3.\n"
                  "Your base character stats are: \n"
                  "\n")
          .arg(m_signUpData->gender, m_signUpData->race->adjective(),
               m_signUpData->characterClass->name()) +
          QString("  %1, %2, %3, %4, %5, %6.\n")
          .arg(Util::highlight(QString("STR: %1").arg(m_signUpData->stats.strength)),
               Util::highlight(QString("DEX: %1").arg(m_signUpData->stats.dexterity)),
               Util::highlight(QString("VIT: %1").arg(m_signUpData->stats.vitality)),
               Util::highlight(QString("END: %1").arg(m_signUpData->stats.endurance)),
               Util::highlight(QString("INT: %1").arg(m_signUpData->stats.intelligence)),
               Util::highlight(QString("FAI: %1").arg(m_signUpData->stats.faith))) +
          "\n"
          "You may assign an additional 9 points freely over your various attributes.\n");
}

void Session::askForExtraStats() {

    bool barbarian = (m_signUpData->characterClass->name() == "Barbarian");

    write(QString("\n"
                  "Please enter the distribution you would like to use in the following form:\n"
                  "\n"
                  "  %1\n"
                  "  (Replace every part with a number, for a total of 9. Example: %2)\n"
                  "\n"
                  "To revisit your choice of gender, type %3. If you want more information about "
                  "character stats, type %4.\n")
          .arg(Util::highlight(barbarian ? "<str> <dex> <vit> <end> <fai>" :
                                           "<str> <dex> <vit> <end> <int> <fai>"),
               Util::highlight(barbarian ? "2 2 2 2 1" : "2 2 2 1 1 1"),
               Util::highlight("back"),
               Util::highlight("info stats")));
}

void Session::processExtraStats(const QString &answer) {

    if (answer == "info stats") {
        write(QString("\n"
                      "Your character has several attributes, each of which will have a value "
                      "assigned. Collectively, we call these your character stats. Here is an "
                      "overview:\n"
                      "\n"
                      "%1 (STR)\n"
                      "  Strength primarily determines the power of your physical attacks. When\n"
                      "  wielding a shield, it also gives a small defense power up.\n"
                      "\n"
                      "%2 (DEX)\n"
                      "  Dexterity determines the speed with which attacks can be dealt. It also \n"
                      "  improves your chances of evading enemy attacks, and the chance of success "
                      "when\n"
                      "  fleeing.\n"
                      "\n"
                      "%3 (VIT)\n"
                      "  Vitality primarily determines your max. health points (HP).\n"
                      "\n"
                      "%4 (END)\n"
                      "  Endurance primarily determines your physical defense power.\n"
                      "\n"
                      "%5 (INT)\n"
                      "  Intelligence determines your max. magic points (MP).\n"
                      "\n"
                      "%6 (FAI)\n"
                      "  Faith determines the magical defense power. It also decreases the chance "
                      "that\n"
                      "  a spell will fail when cast.\n")
              .arg(Util::highlight("Strength"), Util::highlight("Dexterity"),
                   Util::highlight("Vitality"), Util::highlight("Endurance"),
                   Util::highlight("Intelligence"), Util::highlight("Faith")));
    } else if (answer == "back" || answer == "b") {
        setSignInStage(AskingGender);
    } else {
        bool barbarian = (m_signUpData->characterClass->name() == "Barbarian");

        QStringList attributes = answer.split(' ', QString::SkipEmptyParts);
        if (attributes.length() != (barbarian ? 5 : 6)) {
            return;
        }

        CharacterStats stats;
        stats.strength = qMax(attributes[0].toInt(), 0);
        stats.dexterity = qMax(attributes[1].toInt(), 0);
        stats.vitality = qMax(attributes[2].toInt(), 0);
        stats.endurance = qMax(attributes[3].toInt(), 0);
        stats.intelligence = barbarian ? 0 : qMax(attributes[4].toInt(), 0);
        stats.faith = qMax(attributes[barbarian ? 4 : 5].toInt(), 0);

        if (stats.total() != 9) {
            write(Util::colorize("\nThe total of attributes should be 9.\n", Maroon));
            return;
        }

        m_signUpData->stats += stats;

        m_signUpData->height += stats.intelligence - (stats.dexterity / 2);
        m_signUpData->weight += stats.strength;

        write(Util::colorize("\nYour character stats have been recorded.\n", Green));
        setSignInStage(AskingSignupConfirmation);
    }
}

void Session::startAskingSignupConfirmation() {

    write(QString("\n"
                  "You have selected to become a %1 %2 %3.\n"
                  "Your final character stats are: \n"
                  "\n")
          .arg(m_signUpData->gender,
               m_signUpData->race->adjective(),
               m_signUpData->characterClass->name()) +
          QString("  %1, %2, %3, %4, %5, %6.\n")
          .arg(Util::highlight(QString("STR: %1").arg(m_signUpData->stats.strength)),
               Util::highlight(QString("DEX: %1").arg(m_signUpData->stats.dexterity)),
               Util::highlight(QString("VIT: %1").arg(m_signUpData->stats.vitality)),
               Util::highlight(QString("END: %1").arg(m_signUpData->stats.endurance)),
               Util::highlight(QString("INT: %1").arg(m_signUpData->stats.intelligence)),
               Util::highlight(QString("FAI: %1").arg(m_signUpData->stats.faith))));
}

void Session::askForSignupConfirmation() {

    write("\n"
          "Are you ready to create a character with these stats?\n");
}

void Session::processSignupConfirmation(const QString &answer) {

    if (answer == "yes" || answer == "y") {
        QString salt = Util::randomString(8);
        QByteArray data = QString(salt + m_signUpData->password).toUtf8();
        QString hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();

        m_player = GameObject::createByObjectType<Player *>(m_realm, "player");

        m_player->setAdmin(m_realm->players().isEmpty());
        m_player->setName(m_signUpData->userName);
        m_player->setPasswordSalt(salt);
        m_player->setPasswordHash(hash);
        m_player->setRace(m_signUpData->race);
        m_player->setClass(m_signUpData->characterClass);
        m_player->setGender(m_signUpData->gender);
        m_player->setStats(m_signUpData->stats);
        m_player->setHeight(m_signUpData->height);
        m_player->setWeight(m_signUpData->weight);
        m_player->setCurrentArea(m_signUpData->race->startingArea());

        m_player->setHp(m_player->maxHp());
        m_player->setMp(m_player->maxMp());
        m_player->setGold(100);

        delete m_signUpData;
        m_signUpData = 0;

        write(QString("\nWelcome to " GAME_TITLE ", %1.\n").arg(m_player->name()));
        setSignInStage(SignedIn);
    } else if (answer == "no" || answer == "n" ||
               answer == "back" || answer == "b") {
        setSignInStage(AskingExtraStats);
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
