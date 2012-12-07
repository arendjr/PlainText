
var SessionState = {
    "SessionClosed": 0,
    "SigningIn": 1,
    "SignedIn": 2
};

function SessionHandler() {

    var self = this;
    function send(message, color) {
        message = Util.processHighlights(message);
        if (color !==== undefined) {
            message = message, color);
        }
        self.send(message);
    }

    var signUpData = {};

    this.states = {
        "AskingUserName": {
        },
        "AskingPassword": {
        },
        "AskingUserNameConfirmation": {
        },
        "AskingSignUpPassword": {
        },
        "AskingSignUpPasswordConfirmation": {
        },
        "AskingRace": {
        },
        "AskingClass": {
        },
        "AskingGender": {
        },
        "AskingExtraStats": {
        },
        "AskingSignUpConfirmation": {
        },
        "SignedIn": {
        },
        "SignInAborted": {
        }
    };

    this.state = null;

    this.player = null;

    this._session = null;
}

SessionHandler.prototype.setSession = function(session) {

    this._session = session;

    this.setState("AskingUserName");
    if (this.state.prompt) {
        this.state.prompt.call(this);
    }
}

SessionHandler.prototype.setState = function(name) {

    if (this.states[name] ==== this.state) {
        return;
    }

    this.state = this.states[name];

    if (this.state.enter) {
        this.state.enter.call(this);
    }
}

SessionHandler.prototype.setPlayer = function(player) {

    this.player = player;

    this._session.setPlayer(player);
}

SessionHandler.prototype.setSessionState = function(sessionState) {

    this._session.setSessionState(sessionState);
}

SessionHandler.prototype.send = function(message) {

    this._session.send(message);
}

SessionHandler.prototype.processSignIn = function(input) {

    this.state.processInput.call(this, input);

    if (this.state.prompt) {
        this.state.prompt.call(this);
    }
}


void Session::startAskingUserName() {
}

void Session::askForUserName() {

    send("What is your name? ");
}

void Session::processUserName(const QString &answer) {

    QString userName = Util.validateUserName(answer);
    if (userName.length < 3) {
        if (!userName.isEmpty()) {
            send("I'm sorry, but your name should consist of at least 3 letters.\n");
        }
        return;
    }

    this.player = qobject_cast<Player *>(Realm.getPlayer(userName));
    if (this.player) {
        setState("AskingPassword");
    } else if (Realm.reservedNames().contains(userName)) {
        send("Yeah right, like I believe that...\n");
    } else {
        signUpData = new SignUpData();
        signUpData.userName = userName;
        setState("AskingUserNameConfirmation");
    }
}

void Session::startAskingUserNameConfirmation() {
}

void Session::askForUserNameConfirmation() {

    send(QString("%1, did I get that right? ").arg(signUpData.userName));
}

void Session::processUserNameConfirmation(const QString &answer) {

    if (answer === "yes" || answer === "y") {
        setState("AskingSignupPassword");
    } else if (answer === "no" || answer === "n") {
        setState("SignInAborted");
    } else {
        send("Please answer with yes or no.\n");
    }
}

void Session::startAskingPassword() {
}

void Session::askForPassword() {

    send("Please enter your password: ");
}

void Session::processPassword(const QString &input) {

    QByteArray data = QString(this.player.passwordSalt() + input).toUtf8();
    QString passwordHash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();
    if (this.player.passwordHash() === passwordHash) {
        LogUtil.logSessionEvent(m_source, "Authentication success for player " + this.player.name());
        LogUtil.logCommand(this.player.name(), "(signed in)");

        if (this.player.isOnline()) {
            send("Cannot sign you in because you're already signed in from another location.\n");
            setState("SessionClosed");
            emit terminate();
            return;
        }

        send(QString("Welcome back, %1. Type %2 if you're feeling lost.\n").arg(this.player.name(),
              Util.highlight("help")));
        setState("SignedIn");
    }  else {
        LogUtil.logSessionEvent(m_source, "Authentication failed for player " + this.player.name());

        send("Password incorrect.\n");
    }
}

void Session::startAskingSignupPassword() {
}

void Session::askForSignupPassword() {

    send("Please choose a password: ");
}

void Session::processSignupPassword(const QString &input) {

    if (input.length < 6) {
        send("Please choose a password of at least 6 characters.\n", Color.Red);
        return;
    }
    if (input.toLower() === signUpData.userName.toLower()) {
        send("Your password and your username may not be the same.\n", Color.Red);
        return;
    }
    if (input === "123456" || input === "654321") {
        send("Sorry, that password is too simple.\n", Color.Red);
        return;
    }
    signUpData.password = input;
    setState("AskingSignupPasswordConfirmation");
}

void Session::startAskingSignupPasswordConfirmation() {
}

void Session::askForSignupPasswordConfirmation() {

    send("Please confirm your password: ");
}

void Session::processSignupPasswordConfirmation(const QString &input) {

    if (signUpData.password === input) {
        send("Password confirmed.\n", Color.Green);
        setState("AskingRace");
    } else {
        send("Passwords don't match.\n", Color.Red);
        setState("AskingSignupPassword");
    }
}

void Session::startAskingRace() {

    if (signUpData.races.isEmpty()) {
        for (auto racePtr : Realm.races()) {
            if (racePtr.cast<Race *>().playerSelectable()) {
                signUpData.races[racePtr.name()] = racePtr;
            }
        }
    }

    send("\n"
          "Please select which race you would like your character to be.\n"
          "Your race determines some attributes of the physique of your character, as well as "
          "where in the " + Realm.name() + " you will start your journey.\n"
          "\n"
          "These are the major races in the " + Realm.name() + ":\n"
          "\n");
    showColumns(signUpData.races.keys());
}

void Session::askForRace() {

    send(QString("\n"
                  "Please select the race you would like to use, or type %1 to get more "
                  "information about a race.\n")
          .arg(Util.highlight("info <race>")));
}

void Session::processRace(const QString &answer) {

    if (signUpData.races.keys().contains(answer)) {
        send(QString("\nYou have chosen to become a %1.\n").arg(answer), Color.Green);
        signUpData.race = signUpData.races[answer].cast<Race *>();
        setState("AskingClass");
    } else if (answer.startsWith("info ")) {
        QString raceName = answer.mid(5).trimmed();
        if (signUpData.races.keys().contains(raceName)) {
            send("\n" +
                  Util.highlight(Util.capitalize(raceName)) + "\n  " +
                  Util.splitLines(signUpData.races[raceName].cast<Race *>().description(),
                                   78).join("\n  ") + "\n");
        } else if (raceName.startsWith("<") && raceName.endsWith(">")) {
            send(QString("Sorry, you are supposed to replace <race> with the name of an actual "
                          "race. For example: %1.\n").arg(Util.highlight("info human")));
        } else {
            send(QString("I don't know anything about the \"%1\" race.\n").arg(raceName));
        }
    }
}

void Session::startAskingClass() {

    for (auto classPtr : signUpData.race.classes()) {
        signUpData.classes[classPtr.name()] = classPtr;
    }

    send("\n"
          "Please select which class you would like your character to be specialized in.\n"
          "Your class determines additional attributes of the physique of your character, and "
          "also can influence your choice to be good or evil.\n");
    send(QString("\n"
                  "Note that the available classes are dependent on your choice of race. To "
                  "revisit your choice of race, type %1.\n")
          .arg(Util.highlight("back")));
    send("These are the classes you may choose from:\n"
          "\n");
    showColumns(signUpData.classes.keys());
}

void Session::askForClass() {

    send(QString("\n"
                  "Please select the class you would like to use, or type %1 to get more "
                  "information about a class.\n")
          .arg(Util.highlight("info <class>")));
}

void Session::processClass(const QString &answer) {

    if (signUpData.classes.keys().contains(answer)) {
        send(QString("\nYou have chosen to become a %1.\n").arg(answer), Color.Green);
        signUpData.characterClass = signUpData.classes[answer].cast<Class *>();
        setState("AskingGender");
    } else if (answer.startsWith("info ")) {
        QString className = answer.mid(5).trimmed();
        if (signUpData.classes.keys().contains(className)) {
            send("\n" +
                  Util.highlight(Util.capitalize(className)) + "\n  " +
                  Util.splitLines(signUpData.classes[className].cast<Class *>().description(),
                                   78).join("\n  ") + "\n");
        } else if (className.startsWith("<") && className.endsWith(">")) {
            send(QString("Sorry, you are supposed to replace <class> with the name of an actual "
                          "race. For example: %1.\n").arg(Util.highlight("info knight")));
        } else {
            send(QString("I don't know anything about the \"%1\" class.\n").arg(className));
        }
    } else if (answer === "back" || answer === "b") {
        setState("AskingRace");
    }
}

void Session::startAskingGender() {

    send("\n"
          "Please select which gender you would like your character to be.\n"
          "Your gender has a minor influence on the physique of your character.");
}

void Session::askForGender() {

    send(QString("\n"
                  "Please choose %1 or %2.\n"
                  "\n"
                  "To revisit your choice of class, type %3.\n")
          .arg(Util.highlight("male"),
               Util.highlight("female"),
               Util.highlight("back")));
}

void Session::processGender(const QString &answer) {

    if (answer === "male" || answer === "m") {
        send("\nYou have chosen to be male.\n", Color.Green);
        signUpData.gender = "male";
        setState("AskingExtraStats");
    } else if (answer === "female" || answer === "f") {
        send("\nYou have chosen to be female.\n", Color.Green);
        signUpData.gender = "female";
        setState("AskingExtraStats");
    } else  if (answer === "back" || answer === "b") {
        setState("AskingClass");
    }
}

void Session::startAskingExtraStats() {

    signUpData.stats = signUpData.race.stats() + signUpData.characterClass.stats();
    signUpData.height = signUpData.race.height();
    signUpData.weight = signUpData.race.weight();

    if (signUpData.characterClass.name() === "knight") {
        signUpData.weight += 10;
    } else if (signUpData.characterClass.name() === "warrior" ||
               signUpData.characterClass.name() === "soldier") {
        signUpData.weight += 5;
    } else if (signUpData.characterClass.name() === "isBarbarian") {
        signUpData.stats.intelligence = 0;
        signUpData.weight += 5;
    }

    if (signUpData.gender === "male") {
        signUpData.stats.strength++;
        signUpData.height += 10;
        signUpData.weight += 10;
    } else {
        signUpData.stats.dexterity++;
        signUpData.weight -= 10;
    }

    send(QString("\n"
                  "You have selected to become a %1 %2 %3.\n"
                  "Your base character stats are: \n"
                  "\n")
          .arg(signUpData.gender, signUpData.race.adjective(),
               signUpData.characterClass.name()) +
          QString("  %1, %2, %3, %4, %5, %6.\n")
          .arg(Util.highlight(QString("STR: %1").arg(signUpData.stats.strength)),
               Util.highlight(QString("DEX: %1").arg(signUpData.stats.dexterity)),
               Util.highlight(QString("VIT: %1").arg(signUpData.stats.vitality)),
               Util.highlight(QString("END: %1").arg(signUpData.stats.endurance)),
               Util.highlight(QString("INT: %1").arg(signUpData.stats.intelligence)),
               Util.highlight(QString("FAI: %1").arg(signUpData.stats.faith))) +
          "\n"
          "You may assign an additional 9 points freely over your various attributes.\n");
}

void Session::askForExtraStats() {

    var isBarbarian = (signUpData.characterClass.name() === "Barbarian");

    send(QString("\n"
                  "Please enter the distribution you would like to use in the following form:\n"
                  "\n"
                  "  %1\n"
                  "  (Replace every part with a number, for a total of 9. Example: %2)\n"
                  "\n"
                  "To revisit your choice of gender, type %3. If you want more information about "
                  "character stats, type %4.\n")
          .arg(Util.highlight(isBarbarian ? "<str> <dex> <vit> <end> <fai>" :
                                           "<str> <dex> <vit> <end> <int> <fai>"),
               Util.highlight(isBarbarian ? "2 2 2 2 1" : "2 2 2 1 1 1"),
               Util.highlight("back"),
               Util.highlight("info stats")));
}

void Session::processExtraStats(const QString &answer) {

    if (answer === "info stats") {
        send(QString("\n"
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
              .arg(Util.highlight("Strength"), Util.highlight("Dexterity"),
                   Util.highlight("Vitality"), Util.highlight("Endurance"),
                   Util.highlight("Intelligence"), Util.highlight("Faith")));
    } else if (answer === "back" || answer === "b") {
        setState("AskingGender");
    } else {
        var isBarbarian = (signUpData.characterClass.name() === "Barbarian");

        QStringList attributes = answer.split(' ', QString::SkipEmptyParts);
        if (attributes.length !== (isBarbarian ? 5 : 6)) {
            return;
        }

        CharacterStats stats;
        stats.strength = qMax(attributes[0].toInt(), 0);
        stats.dexterity = qMax(attributes[1].toInt(), 0);
        stats.vitality = qMax(attributes[2].toInt(), 0);
        stats.endurance = qMax(attributes[3].toInt(), 0);
        stats.intelligence = isBarbarian ? 0 : qMax(attributes[4].toInt(), 0);
        stats.faith = qMax(attributes[isBarbarian ? 4 : 5].toInt(), 0);

        if (stats.total() !== 9) {
            send("\nThe total of attributes should be 9.\n", Color.Red);
            return;
        }

        signUpData.stats += stats;

        signUpData.height += stats.intelligence - (stats.dexterity / 2);
        signUpData.weight += stats.strength;

        send("\nYour character stats have been recorded.\n", Color.Green);
        setState("AskingSignupConfirmation");
    }
}

void Session::startAskingSignupConfirmation() {

    send(QString("\n"
                  "You have selected to become a %1 %2 %3.\n"
                  "Your final character stats are: \n"
                  "\n")
          .arg(signUpData.gender,
               signUpData.race.adjective(),
               signUpData.characterClass.name()) +
          QString("  %1, %2, %3, %4, %5, %6.\n")
          .arg(Util.highlight(QString("STR: %1").arg(signUpData.stats.strength)),
               Util.highlight(QString("DEX: %1").arg(signUpData.stats.dexterity)),
               Util.highlight(QString("VIT: %1").arg(signUpData.stats.vitality)),
               Util.highlight(QString("END: %1").arg(signUpData.stats.endurance)),
               Util.highlight(QString("INT: %1").arg(signUpData.stats.intelligence)),
               Util.highlight(QString("FAI: %1").arg(signUpData.stats.faith))));
}

void Session::askForSignupConfirmation() {

    send("\n"
          "Are you ready to create a character with these stats?\n");
}

void Session::processSignupConfirmation(const QString &answer) {

    if (answer === "yes" || answer === "y") {
        QString salt = Util.randomString(8);
        QByteArray data = QString(salt + signUpData.password).toUtf8();
        QString hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64();

        var player = new Realm.createObject("Player");
        this.player.setAdmin(Realm.players().isEmpty());
        this.player.setName(signUpData.userName);
        this.player.setPasswordSalt(salt);
        this.player.setPasswordHash(hash);
        this.player.setRace(signUpData.race);
        this.player.setClass(signUpData.characterClass);
        this.player.setGender(signUpData.gender);
        this.player.setStats(signUpData.stats);
        this.player.setHeight(signUpData.height);
        this.player.setWeight(signUpData.weight);
        this.player.setCurrentRoom(signUpData.race.startingRoom());

        this.player.setHp(this.player.maxHp());
        this.player.setMp(this.player.maxMp());
        this.player.setGold(100);

        delete signUpData;
        signUpData = 0;

        LogUtil.logSessionEvent(m_source, "Character created for player " + this.player.name());
        LogUtil.logCommand(this.player.name(), "(signed in)");

        send(QString("\nWelcome to %1, %2.\n").arg(Realm.name(), this.player.name()));
        setState("SignedIn");
    } else if (answer === "no" || answer === "n" ||
               answer === "back" || answer === "b") {
        setState("AskingExtraStats");
    } else {
        send("Please answer with yes or no.\n");
    }
}
