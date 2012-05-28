#include "session.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QMap>

#include "engine/area.h"
#include "engine/characterstats.h"
#include "engine/class.h"
#include "engine/commandinterpreter.h"
#include "engine/constants.h"
#include "engine/gameobjectptr.h"
#include "engine/player.h"
#include "engine/race.h"
#include "engine/realm.h"
#include "engine/util.h"


class Session::SignUpData {

    public:
        QString userName;
        QString password;

        Race *race;
        Class *characterClass;
        QString gender;
        CharacterStats stats;

        SignUpData() :
            race(0),
            characterClass(0) {
        }
};

Session::Session(QObject *parent) :
    QObject(parent),
    m_signInStage(SessionClosed),
    m_signUpData(0),
    m_player(0) {
}

Session::~Session() {

    if (m_player) {
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
    QString answer = input.toLower();

    QMap<QString, GameObjectPtr> races;
    QMap<QString, GameObjectPtr> classes;
    if (m_signInStage == AskingRace) {
        foreach (const GameObjectPtr &racePtr, Realm::instance()->races()) {
            races[racePtr->name()] = racePtr;
        }
    } else if (m_signInStage == AskingClass) {
        foreach (const GameObjectPtr &classPtr, m_signUpData->race->classes()) {
            classes[classPtr->name()] = classPtr;
        }
    }

    bool barbarian = (m_signUpData &&
                      m_signUpData->characterClass &&
                      m_signUpData->characterClass->name() == "Barbarian");

    SignInStage previousSignInStage = m_signInStage;

    QString passwordHash;
    switch (m_signInStage) {
        case AskingUserName: {
            QString userName = Util::capitalize(answer.left(12));
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
        case AskingUserNameConfirmation: {
            if (answer == "yes" || answer == "y") {
                m_signInStage = AskingSignupPassword;
            } else if (answer == "no" || answer == "n") {
                m_signInStage = SignInAborted;
            } else {
                write("Please answer with yes or no.\n");
            }
            break;
        }
        case AskingPassword:
            passwordHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toBase64();
            if (m_player->passwordHash() == passwordHash) {
                write(QString("Welcome back, %1. Type %2 if you're feeling lost.\n")
                      .arg(m_player->name(), Util::highlight("help")));
                m_signInStage = SignedIn;
            }  else {
                write("Password incorrect.\n");
            }
            break;

        case AskingSignupPassword:
            if (input.length() < 6) {
                write(Util::colorize("Please choose a password of at least 6 characters.\n", Maroon));
                break;
            }
            if (input.toLower() == m_signUpData->userName.toLower()) {
                write(Util::colorize("Your password and your username may not be the same.\n", Maroon));
                break;
            }
            if (input == "123456" || input == "654321") {
                write(Util::colorize("Sorry, that password is too simple.\n", Maroon));
                break;
            }
            m_signUpData->password = input;
            m_signInStage = AskingSignupPasswordConfirmation;
            break;

        case AskingSignupPasswordConfirmation:
            if (m_signUpData->password == input) {
                write(Util::colorize("Password confirmed.\n", Green));
                m_signInStage = AskingRace;
            } else {
                write(Util::colorize("Passwords don't match.\n", Maroon));
                m_signInStage = AskingSignupPassword;
            }
            break;

        case AskingRace:
            if (races.keys().contains(answer)) {
                write(Util::colorize(QString("\nYou have chosen to become a %1.\n").arg(answer), Green));
                m_signUpData->race = races[answer].cast<Race *>();
                m_signInStage = AskingClass;
            } else if (answer.startsWith("info ")) {
                QString raceName = answer.mid(5).trimmed();
                if (races.keys().contains(raceName)) {
                    write("\n" +
                          Util::highlight(Util::capitalize(raceName)) + "\n"
                          "  " + Util::splitLines(races[raceName].cast<Race *>()->description(), 78).join("\n  ") + "\n");
                } else {
                    write(QString("I don't know anything about the \"%1\" race.\n").arg(raceName));
                }
            }
            break;

        case AskingClass:
            if (classes.keys().contains(answer)) {
                write(Util::colorize(QString("\nYou have chosen to become a %1.\n").arg(answer), Green));
                m_signUpData->characterClass = classes[answer].cast<Class *>();
                m_signInStage = AskingGender;
            } else if (answer.startsWith("info ")) {
                QString className = answer.mid(5).trimmed();
                if (classes.keys().contains(className)) {
                    write("\n" +
                          Util::highlight(Util::capitalize(className)) + "\n"
                          "  " + Util::splitLines(classes[className].cast<Class *>()->description(), 78).join("\n  ") + "\n");
                } else {
                    write(QString("I don't know anything about the \"%1\" class.\n").arg(className));
                }
            } else if (answer == "back" || answer == "b") {
                m_signInStage = AskingRace;
            }
            break;

        case AskingGender:
            if (answer == "male" || answer == "m") {
                write(Util::colorize("\nYou have chosen to be male.\n", Green));
                m_signUpData->gender = "male";
                m_signInStage = AskingExtraStats;
            } else if (answer == "female" || answer == "f") {
                write(Util::colorize("\nYou have chosen to be female.\n", Green));
                m_signUpData->gender = "female";
                m_signInStage = AskingExtraStats;
            } else  if (answer == "back" || answer == "b") {
                m_signInStage = AskingClass;
            }
            break;

        case AskingExtraStats:
            if (answer == "info stats") {
                write(QString("\n"
                              "Your character has several attributes, each of which will have a value assigned. "
                              "Collectively, we call these your character stats. Here is an overview:\n"
                              "\n"
                              "%1 (STR)\n"
                              "  Strength primarily determines the power of your physical attacks. When\n"
                              "  wielding a shield, it also gives a small defence power up.\n"
                              "\n"
                              "%2 (DEX)\n"
                              "  Dexterity determines the speed with which attacks can be dealt. It also \n"
                              "  improves your chances of evading enemy attacks, and the chance of success when\n"
                              "  fleeing.\n"
                              "\n"
                              "%3 (VIT)\n"
                              "  Vitality primarily determines your max. health points (HP).\n"
                              "\n"
                              "%4 (END)\n"
                              "  Endurance primarily determines your physical defence power.\n"
                              "\n"
                              "%5 (INT)\n"
                              "  Intelligence determines your max. magic points (MP).\n"
                              "\n"
                              "%6 (FAI)\n"
                              "  Faith determines the magical defence power. It also decreases the chance that\n"
                              "  a spell will fail when cast.\n")
                      .arg(Util::highlight("Strength"), Util::highlight("Dexterity"),
                           Util::highlight("Vitality"), Util::highlight("Endurance"),
                           Util::highlight("Intelligence"), Util::highlight("Faith")));
            } else if (answer == "back" || answer == "b") {
                m_signInStage = AskingGender;
            } else {
                QStringList attributes = answer.split(' ', QString::SkipEmptyParts);
                if (attributes.length() != (barbarian ? 5 : 6)) {
                    break;
                }

                uint strength = attributes[0].toUInt();
                uint dexterity = attributes[1].toUInt();
                uint vitality = attributes[2].toUInt();
                uint endurance = attributes[3].toUInt();
                uint intelligence = barbarian ? 0 : attributes[4].toUInt();
                uint faith = attributes[barbarian ? 4 : 5].toUInt();

                uint total = strength + dexterity + vitality +
                            endurance + intelligence + faith;
                if (total != 9) {
                    write(Util::colorize("\nThe total of attributes should be 9.\n", Maroon));
                    break;
                }

                m_signUpData->stats.strength += strength;
                m_signUpData->stats.dexterity += dexterity;
                m_signUpData->stats.vitality += vitality;
                m_signUpData->stats.endurance += endurance;
                m_signUpData->stats.intelligence += intelligence;
                m_signUpData->stats.faith += faith;

                m_signUpData->stats.height += intelligence - (dexterity / 2);
                m_signUpData->stats.weight += strength;

                write(Util::colorize("\nYour character stats have been recorded.\n", Green));
                m_signInStage = AskingSignupConfirmation;
            }
            break;

        case AskingSignupConfirmation:
            if (answer == "yes" || answer == "y") {
                passwordHash = QCryptographicHash::hash(m_signUpData->password.toUtf8(), QCryptographicHash::Sha1).toBase64();
                m_player = qobject_cast<Player *>(GameObject::createByObjectType("player"));
                Q_ASSERT(m_player);

                m_player->startBulkModification();

                m_player->setName(m_signUpData->userName);
                m_player->setPasswordHash(passwordHash);
                m_player->setRace(m_signUpData->race);
                m_player->setClass(m_signUpData->characterClass);
                m_player->setGender(m_signUpData->gender);
                m_player->setStats(m_signUpData->stats);
                m_player->setCurrentArea(m_signUpData->race->startingArea());

                m_player->setHp(m_player->maxHp());
                m_player->setMp(m_player->maxMp());
                m_player->setGold(100);

                m_player->commitBulkModification();

                delete m_signUpData;
                m_signUpData = 0;

                write(QString("\nWelcome to " GAME_TITLE ", %1.\n").arg(m_player->name()));
                m_signInStage = SignedIn;
            } else if (answer == "no" || answer == "n" ||
                       answer == "back" || answer == "b") {
                m_signInStage = AskingExtraStats;
            } else {
                write("Please answer with yes or no.\n");
            }
            break;

        default:
            break;
    }

    if (m_signInStage == AskingRace && races.isEmpty()) {
        foreach (const GameObjectPtr &racePtr, Realm::instance()->races()) {
            races[racePtr->name()] = racePtr;
        }
    } else if (m_signInStage == AskingClass && classes.isEmpty()) {
        foreach (const GameObjectPtr &classPtr, m_signUpData->race->classes()) {
            classes[classPtr->name()] = classPtr;
        }
    } else if (m_signInStage == AskingExtraStats) {
        CharacterStats raceStats = m_signUpData->race->stats();
        CharacterStats classStats = m_signUpData->characterClass->stats();

        m_signUpData->stats.strength = raceStats.strength + classStats.strength;
        m_signUpData->stats.dexterity = raceStats.dexterity + classStats.dexterity;
        m_signUpData->stats.vitality = raceStats.vitality + classStats.vitality;
        m_signUpData->stats.endurance = raceStats.endurance + classStats.endurance;
        m_signUpData->stats.intelligence = raceStats.intelligence + classStats.intelligence;
        m_signUpData->stats.faith = raceStats.faith + classStats.faith;

        m_signUpData->stats.height = raceStats.height + classStats.height;
        m_signUpData->stats.weight = raceStats.weight + classStats.weight;

        if (barbarian) {
            m_signUpData->stats.intelligence = 0;
        }
        if (m_signUpData->gender == "male") {
            m_signUpData->stats.strength++;
            m_signUpData->stats.height += 10;
            m_signUpData->stats.weight += 10;
        } else {
            m_signUpData->stats.dexterity++;
            m_signUpData->stats.weight -= 10;
        }
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
            write("Please confirm your password: ");
            break;

        case AskingRace:
            if (previousSignInStage != AskingRace) {
                write("\n"
                      "Please select which race you would like your character to be.\n"
                      "Your race determines some attributes of the physique of your character, as "
                      "well as where in the " GAME_TITLE " you will start your journey.\n"
                      "\n"
                      "These are the major races in the " GAME_TITLE ":\n"
                      "\n");
                showColumns(races.keys());
            }
            write(QString("\n"
                          "Please select the race you would like to use, or type %1 to get more information "
                          "about a race.\n")
                  .arg(Util::highlight("info <race>")));
            break;

        case AskingClass:
            if (previousSignInStage != AskingClass) {
                write("\n"
                      "Please select which class you would like your character to be specialized in.\n"
                      "Your class determines additional attributes of the physique of your character, "
                      "and also can influence your choice to be good or evil.\n");
                write(QString("\n"
                              "Note that the available classes are dependent on your choice of race. To "
                              "revisit your choice of race, type %1.\n")
                      .arg(Util::highlight("back")));
                write("These are the classes you may choose from:\n"
                      "\n");
                showColumns(classes.keys());
            }
            write(QString("\n"
                          "Please select the class you would like to use, or type %1 to get more "
                          "information about a class.\n")
                  .arg(Util::highlight("info <class>")));
            break;

        case AskingGender:
            if (previousSignInStage != AskingGender) {
                write("\n"
                      "Please select which gender you would like your character to be.\n"
                      "Your gender has a minor influence on the physique of your character.");
            }
            write(QString("\n"
                          "Please choose %1 or %2.\n"
                          "\n"
                          "To revisit your choice of class, type %3.\n")
                  .arg(Util::highlight("male"),
                       Util::highlight("female"),
                       Util::highlight("back")));
            break;

        case AskingExtraStats:
            if (previousSignInStage != AskingExtraStats) {
                write(QString("\n"
                              "You have selected to become a %1 %2 %3.\n"
                              "Your base character stats are: \n"
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
                           Util::highlight(QString("FAI: %1").arg(m_signUpData->stats.faith))) +
                      "\n"
                      "You may assign an additional 9 points freely over your various attributes.\n");
            }
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
            break;

        case AskingSignupConfirmation:
            if (previousSignInStage != AskingSignupConfirmation) {
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
            write("\n"
                  "Are you ready to create a character with these stats?\n");
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
