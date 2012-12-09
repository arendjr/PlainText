function combat(attacker, defendant, observers) {

    var attackerStats = attacker.totalStats();
    var defendantStats = defendant.totalStats();

    var hitChance = 100 * ((80 + attackerStats[DEXTERITY]) / 160.0) *
                          ((100 - defendantStats[DEXTERITY]) / 100.0);
    var damage = 0;
    if (byChance(hitChance, 100)) {
        damage = randomInt(1, 20.0 * (attackerStats[STRENGTH] / 40.0) *
                                     ((80 - defendantStats[ENDURANCE]) / 80.0));

        defendant.hp -= damage;
    }

    var weaponCategory = (attacker.weapon ? attacker.weapon.category : "");
    var secondaryWeaponCategory = (attacker.secondaryWeapon ? attacker.secondaryWeapon.category :
                                                              "");

    var weapon;
    function attackerIsWieldingA(category) {
        if (weaponCategory === category) {
            return attacker.weapon;
        } else if (secondaryWeaponCategory === category) {
            return attacker.secondaryWeapon;
        } else {
            return null;
        }
    }

    var beginnings = [];
    var endings = [];

    if (defendant.race && defendant.race.name === "animal" && defendant.weight < 50) {
        beginnings.push("%a kicks toward %d");

        if (damage > 0) {
            if ((weapon = attackerIsWieldingA("stick")) ||
                (weapon = attackerIsWieldingA("warhammer"))) {
                beginnings.push("%a smashes %pa " + weapon.name + " into %d");
                beginnings.push("%a bludgeons %d");
            }

            endings.push("and %d jumps up as you hit %pd chest");
        } else {
            beginnings.push("%a tries to hit %d");

            endings.push("but %d is too fast for %oa.");
        }
    } else {
        if (weaponCategory.isEmpty() && secondaryWeaponCategory.isEmpty()) {
            beginnings.push("%a brings up a mighty punch");
            beginnings.push("%a kicks wildly at %d");
        } else {
            if (!attackerIsWieldingA("warhammer")) {
                beginnings.push("%a thrusts at %d");
            }

            if (attackerIsWieldingA("sword")) {
                beginnings.push("%a slashes at %d");
            } else if (attackerIsWieldingA("spear")) {
                beginnings.push("%a lashes out at %d");
            } else if (attackerIsWieldingA("dagger")) {
                beginnings.push("%a stabs at %d");
            }
        }

        if (damage > 0) {
            if (weaponCategory.isEmpty() && secondaryWeaponCategory.isEmpty()) {
                beginnings.push("%a deals %d a sweeping punch");
            } else {
                if ((weapon = attackerIsWieldingA("stick"))) {
                    beginnings.push("%a smashes %pa " + weapon.name + " into %d");
                    beginnings.push("%a bludgeons %d");
                } else if ((weapon = attackerIsWieldingA("warhammer"))) {
                    beginnings.push("%a smashes %pa " + weapon.name + " into %d");
                    beginnings.push("%a bludgeons %d");
                    beginnings.push("%a crushes %d with %pa " + weapon.name);
                }
            }

            if (attacker.height > defendant.height - 100) {
                endings.push("and hits %od in the face");
                endings.push("and violently smashes %pd nose");
                endings.push("hitting %od on the jaw");
            }

            endings.push("and hits %od in the stomach, causing %od to double over");
            endings.push("and hits %od in the flank");
        } else {
            if (weaponCategory.isEmpty() && secondaryWeaponCategory.isEmpty()) {
                beginnings.push("%a tries to hit %d");
            } else {
                beginnings.push("%a tries to hit %d with %pa %w");
            }

            endings.push("but %d blocks the blow");
            endings.push("but hits nothing but air");
            endings.push("but fails to hurt %od");
        }
    }

    var beginning = beginnings.randomElement();
    var ending = endings.randomElement();

    var message = (ending.startsWith("and") ? beginning + " " + ending : beginning + ", " + ending);

    function secondPersonIndicativeOf(verb) {
        if (verb.endsWith("ies")) {
            return verb.substr(0, verb.length - 3) + "y";
        } else if (verb.endsWith("es")) {
            return verb.substr(0, verb.length - 2);
        } else {
            return verb.substr(0, verb.length - 1);
        }
    }

    var verb = beginning.split(" ")[1];
    var secondPersonIndicative = secondPersonIndicativeOf(verb);

    var secondVerb = undefined;
    var secondVerbPerson = "";
    var endingWords = ending.split(" ");
    if (endingWords[1].endsWith("s")) {
        secondVerb = endingWords[1];
        secondVerbPerson = "attacker";
    } else if (endingWords[2].endsWith("s")) {
        secondVerb = endingWords[2];
        secondVerbPerson = (endingWords[1] == "%d" ? "defendant" : "attacker");
    }
    var secondSecondPersonIndicative = secondVerb ? secondPersonIndicativeOf(secondVerb) : "";

    var area = defendant.currentRoom;

    var attackerDefiniteName = attacker.definiteName(area.npcs);
    var defendantDefiniteName = defendant.definiteName(area.npcs);

    if (attacker.isPlayer()) {
        var attackerMessage = message.replace(verb, secondPersonIndicative);
        if (secondVerbPerson === "attacker") {
            attackerMessage = attackerMessage.replace(secondVerb, secondSecondPersonIndicative);
        }
        attackerMessage = attackerMessage.replace("%a", "you");
        attackerMessage = attackerMessage.replace("%oa", "you");
        attackerMessage = attackerMessage.replace("%pa", "your");
        if (attackerMessage.contains("%d")) {
            attackerMessage = attackerMessage.replace("%d", defendantDefiniteName);
            attackerMessage = attackerMessage.replaceAll("%d", defendant.subjectPronoun);
            attackerMessage = attackerMessage.replaceAll("%od", defendant.objectPronoun);
            attackerMessage = attackerMessage.replaceAll("%pd", defendant.possessiveAdjective);
        } else {
            attackerMessage = attackerMessage.replace("%od", defendantDefiniteName);
            attackerMessage = attackerMessage.replaceAll("%od", defendant.objectPronoun);
            attackerMessage = attackerMessage.replaceAll("%pd", defendantDefiniteName + "'s");
        }
        attackerMessage += (damage > 0 ? ", dealing " + damage + " damage." : ".");

        attacker.send(attackerMessage.capitalized(), Color.Teal);
    }

    if (defendant.isPlayer()) {
        var defendantMessage = message;
        if (secondVerbPerson === "defendant") {
            defendantMessage = defendantMessage.replace(secondVerb, secondSecondPersonIndicative);
        }
        defendantMessage = defendantMessage.replace("%a", attackerDefiniteName);
        defendantMessage = defendantMessage.replace("%oa", attacker.objectPronoun);
        defendantMessage = defendantMessage.replace("%pa", attacker.possessiveAdjective);
        defendantMessage = defendantMessage.replaceAll("%d", "you");
        defendantMessage = defendantMessage.replaceAll("%od", "you");
        defendantMessage = defendantMessage.replaceAll("%pd", "your");
        defendantMessage += (damage > 0 ? ", dealing " + damage + " damage." : ".");

        defendant.send(defendantMessage.capitalized(), damage > 0 ? Color.Red : Color.Green);
    }

    if (!observers.isEmpty()) {
        var observersMessage = message + ".";
        observersMessage = observersMessage.replace("%a", attackerDefiniteName);
        observersMessage = observersMessage.replace("%oa", attacker.objectPronoun);
        observersMessage = observersMessage.replace("%pa", attacker.possessiveAdjective);
        if (observersMessage.contains("%d")) {
            observersMessage = observersMessage.replace("%d", defendantDefiniteName);
            observersMessage = observersMessage.replaceAll("%d", defendant.subjectPronoun);
            observersMessage = observersMessage.replaceAll("%od", defendant.objectPronoun);
            observersMessage = observersMessage.replaceAll("%pd", defendant.possessiveAdjective);
        } else {
            observersMessage = observersMessage.replaceAll("%od", defendantDefiniteName);
            observersMessage = observersMessage.replaceAll("%pd", defendantDefiniteName + "'s");
        }

        observers.send(observersMessage.capitalized(), Color.Teal);
    }

    attacker.stun(4000 - (25 * attackerStats[DEXTERITY]));
}

Realm.setTrigger("oncombat", combat);
