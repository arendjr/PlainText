use lazy_static::lazy_static;
use maplit::hashmap;
use serde_json::json;
use std::cmp;
use std::collections::HashMap;
use std::str::FromStr;

use crate::character_stats::{CharacterStat, CharacterStats};
use crate::colors::Color;
use crate::game_object::GameObject;
use crate::objects::{Class, Gender, Player, Race, Realm};
use crate::sessions::SessionOutput as Output;
use crate::text_utils::{
    capitalize, colorize, format_columns, highlight, is_letter, split_lines, FormatOptions,
};

lazy_static! {
    static ref RESERVED_NAMES: Vec<String> = vec![
        "Admin".to_owned(),
        "Administrator".to_owned(),
        "Root".to_owned()
    ];
    static ref SIGN_IN_STEPS: HashMap<SignInStep, StepImpl> = get_sign_in_steps();
}

#[derive(Clone, Debug)]
pub struct SignInState {
    step: SignInStep,
    data: SignInData,
}

impl SignInState {
    pub fn new() -> Self {
        new_state(
            SignInStep::AskingUserName,
            SignInData {
                class: None,
                gender: Gender::Unspecified,
                height: 0.0,
                password: String::new(),
                player: None,
                race: None,
                stats: CharacterStats::new(),
                user_name: String::new(),
                weight: 0.0,
            },
        )
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
enum SignInStep {
    AskingClass,
    AskingExtraStats,
    AskingGender,
    AskingPassword,
    AskingRace,
    AskingSignUpConfirmation,
    AskingSignUpPassword,
    AskingUserName,
    AskingUserNameConfirmation,
    AskingSignUpPasswordConfirmation,
    SessionClosed,
    SignedIn,
}

#[derive(Clone, Debug)]
struct SignInData {
    class: Option<Class>,
    gender: Gender,
    height: f32,
    password: String,
    player: Option<Player>,
    race: Option<Race>,
    stats: CharacterStats,
    user_name: String,
    weight: f32,
}

struct StepImpl {
    enter: fn(&SignInData, &Realm) -> Output,
    exit: fn(&SignInData) -> Output,
    prompt: fn(&SignInData) -> Output,
    process_input: fn(&SignInState, &Realm, String) -> (SignInState, Output),
}

pub fn process_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Vec<Output>) {
    match SIGN_IN_STEPS.get(&state.step) {
        Some(step) => {
            let (new_state, output) = (step.process_input)(state, realm, input);
            match SIGN_IN_STEPS.get(&new_state.step) {
                Some(new_step) => {
                    let prompt_output = (new_step.prompt)(&new_state.data);
                    if new_state.step == state.step {
                        (new_state, vec![output, prompt_output])
                    } else {
                        let exit_output = (step.exit)(&new_state.data);
                        let enter_output = (new_step.enter)(&new_state.data, realm);
                        (
                            new_state,
                            vec![output, exit_output, enter_output, prompt_output],
                        )
                    }
                }
                None => (
                    new_state,
                    vec![Output::Str("New state has no implementation")],
                ),
            }
        }
        None => (state.clone(), vec![]),
    }
}

fn get_sign_in_steps() -> HashMap<SignInStep, StepImpl> {
    hashmap! {
        SignInStep::AskingUserName => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::None,
            prompt: |_| Output::Str("What is your name? "),
            process_input: process_asking_user_name_input,
        },

        SignInStep::AskingUserNameConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::None,
            prompt: |data| Output::String(format!("{}, did I get that right? ", data.user_name)),
            process_input: process_asking_user_name_confirmation_input,
        },

        SignInStep::AskingSignUpPassword => StepImpl {
            enter: |_, _| Output::JSON(json!({ "inputType": "password" })),
            exit: |_| Output::None,
            prompt: |_| Output::Str("Please choose a password: "),
            process_input: process_asking_sign_up_password_input,
        },

        SignInStep::AskingSignUpPasswordConfirmation => StepImpl {
            enter: |_, _| Output::None,
            exit: |_| Output::JSON(json!({ "inputType": "text" })),
            prompt: |_| Output::Str("Please confirm a password: "),
            process_input: process_asking_sign_up_password_confirmation_input,
        },

        SignInStep::AskingRace => StepImpl {
            enter: enter_race,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the race you would like to use, or type *info <race>* to get \
                more information about a race.\n"
            ),
            process_input: process_asking_race_input
        },

        SignInStep::AskingClass => StepImpl {
            enter: enter_class,
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please select the class you would like to use, or type *info <class>* to \
                get more information about a class.\n"
            ),
            process_input: process_asking_class_input
        },

        SignInStep::AskingGender => StepImpl {
            enter: |_, _| Output::Str(
                "\n\
                Please select which gender you would like your character to be.\n\
                Your gender has a minor influence on the physique of your character."
            ),
            exit: |_| Output::None,
            prompt: |_| Output::Str(
                "\n\
                Please choose *male* or *female*.\n\
                \n\
                To revisit your choice of class, type *back*.\n"
            ),
            process_input: process_asking_gender_input
        },

        SignInStep::AskingExtraStats => StepImpl {
            enter: enter_extra_stats,
            exit: |_| Output::None,
            prompt: |data| Output::String(format!(
                "\n\
                Please enter the distribution you would like to use in the following form:\n\
                \n  *{}*\n  (Replace every part with a number, for a total of 9. Suggestion: {})\n\
                \n\
                To revisit your choice of gender, type *back*. If you want more \
                information about character stats, type *info stats*. If you just want to \
                accept the suggested stats, type *accept*.\n",
                if data.class.as_ref().map(|class| class.get_name()) == Some("barbarian") {
                    "<str> <dex> <vit> <end> <fai>"
                } else {
                    "<str> <dex> <vit> <end> <int> <fai>"
                },
                format_stats_string(&get_base_stats(&data).3, &data.class)

            )),
            process_input: process_asking_extra_stats_input
        },

        SignInStep::AskingSignUpConfirmation => StepImpl {
            enter: enter_sign_up_confirmation,
            exit: |_| Output::None,
            prompt: |_| Output::Str("\nAre you ready to create a character with these stats?\n"),
            process_input: process_asking_sign_up_confirmation_input
        },

        SignInStep::SignedIn => StepImpl {
            enter: |data, realm| Output::String(format!(
                "\nWelcome to {}, {}.\n",
                realm.get_name(),
                data.user_name,
            )),
            exit: |_| Output::None,
            prompt: |_| Output::None,
            process_input: |state, _, _| (state.clone(), Output::None)
        }
    }
}

fn enter_race(_: &SignInData, realm: &Realm) -> Output {
    Output::String(format!(
        "\n\
        Please select which race you would like your character to be.\n\
        \n\
        Your race determines some attributes of the physique of your character, \
        as well as where in the {} you will start your journey.\n\
        \n\
        These are the major races in the {}:\n\
        \n\
        {}",
        realm.get_name(),
        realm.get_name(),
        format_columns(
            realm.race_names(),
            FormatOptions::Capitalized | FormatOptions::Highlighted
        )
    ))
}

fn enter_class(data: &SignInData, realm: &Realm) -> Output {
    Output::String(format!(
        "\n\
        Please select which class you would like your character to be specialized \
        in.\n\
        Your class determines additional attributes of the physique of your \
        character, and also can influence your choice to be good or evil.\n\
        \n\
        Note that the available classes are dependent on your choice of race. To \
        revisit your choice of race, type *back*.\n\
        \n\
        These are the classes you may choose from:\n\
        \n\
        {}",
        match &data.race {
            Some(race) => format_columns(
                race.get_classes()
                    .iter()
                    .filter_map(|object_ref| realm
                        .get(*object_ref)
                        .map(|object| object.get_name().to_owned()))
                    .collect(),
                FormatOptions::Capitalized | FormatOptions::Highlighted
            ),
            None => "(cannot determine classes without race)".to_owned(),
        }
    ))
}

fn enter_extra_stats(data: &SignInData, _: &Realm) -> Output {
    let stats = get_base_stats(data).2;
    Output::String(format!(
        "\n\
        You have selected to become a {} {} {}.\n\
        Your base character stats are:\n\
        \n  *STR: {}*, *DEX: {}*, *VIT: {}*, *END: {}*, *INT: {}*, *FTH: {}*.\n\
        \n\
        You may assign an additional 9 points freely over your various attributes.\n",
        if data.gender == Gender::Male {
            "male"
        } else {
            "female"
        },
        match &data.race {
            Some(race) => race.get_adjective(),
            None => "",
        },
        match &data.class {
            Some(class) => class.get_name(),
            None => "",
        },
        stats.get(CharacterStat::STRENGTH),
        stats.get(CharacterStat::DEXTERITY),
        stats.get(CharacterStat::VITALITY),
        stats.get(CharacterStat::ENDURANCE),
        stats.get(CharacterStat::INTELLIGENCE),
        stats.get(CharacterStat::FAITH),
    ))
}

fn enter_sign_up_confirmation(data: &SignInData, _: &Realm) -> Output {
    Output::String(format!(
        "\n\
        You have selected to become a {} {} {}.\n\
        Your final character stats are: \n\
        \n  *STR: {}*, *DEX: {}*, *VIT: {}*, *END: {}*, *INT: {}*, *FTH: {}*.\n",
        if data.gender == Gender::Male {
            "male"
        } else {
            "female"
        },
        data.race
            .as_ref()
            .map(|race| race.get_adjective())
            .unwrap_or(""),
        data.class
            .as_ref()
            .map(|class| class.get_name())
            .unwrap_or(""),
        data.stats.get(CharacterStat::STRENGTH),
        data.stats.get(CharacterStat::DEXTERITY),
        data.stats.get(CharacterStat::VITALITY),
        data.stats.get(CharacterStat::ENDURANCE),
        data.stats.get(CharacterStat::INTELLIGENCE),
        data.stats.get(CharacterStat::FAITH),
    ))
}

fn format_stats_string(stats: &CharacterStats, class: &Option<Class>) -> String {
    if class.as_ref().map(|class| class.get_name()) == Some("barbarian") {
        format!(
            "{} {} {} {} {}",
            stats.get(CharacterStat::STRENGTH),
            stats.get(CharacterStat::DEXTERITY),
            stats.get(CharacterStat::VITALITY),
            stats.get(CharacterStat::ENDURANCE),
            stats.get(CharacterStat::FAITH)
        )
    } else {
        format!(
            "{} {} {} {} {} {}",
            stats.get(CharacterStat::STRENGTH),
            stats.get(CharacterStat::DEXTERITY),
            stats.get(CharacterStat::VITALITY),
            stats.get(CharacterStat::ENDURANCE),
            stats.get(CharacterStat::INTELLIGENCE),
            stats.get(CharacterStat::FAITH)
        )
    }
}

fn get_base_stats(data: &SignInData) -> (f32, f32, CharacterStats, CharacterStats) {
    let (mut height, mut weight, race_stats, race_stats_suggestion) = match &data.race {
        Some(race) => (
            race.get_height(),
            race.get_weight(),
            race.get_stats(),
            race.get_stats_suggestion(),
        ),
        None => (0.0, 0.0, CharacterStats::new(), CharacterStats::new()),
    };

    let (class_name, class_stats, class_stats_suggestion) = match &data.class {
        Some(class) => (
            class.get_name(),
            class.get_stats(),
            class.get_stats_suggestion(),
        ),
        None => ("", CharacterStats::new(), CharacterStats::new()),
    };

    let mut stats = race_stats + class_stats;
    let mut stats_suggestion = race_stats_suggestion + class_stats_suggestion;

    if class_name == "knight" {
        weight += 10.0;
    } else if class_name == "warrior" || class_name == "soldier" {
        weight += 5.0;
    } else if class_name == "barbarian" {
        stats.set(CharacterStat::INTELLIGENCE, 0);
        weight += 5.0;

        stats_suggestion.set(
            CharacterStat::FAITH,
            stats_suggestion.get(CharacterStat::FAITH)
                + stats_suggestion.get(CharacterStat::INTELLIGENCE),
        );
        stats_suggestion.set(CharacterStat::INTELLIGENCE, 0);
    }

    if data.gender == Gender::Male {
        stats.inc(CharacterStat::STRENGTH);
        height += 10.0;
        weight += 10.0;
    } else {
        stats.inc(CharacterStat::DEXTERITY);
        weight -= 10.0;
    }

    (height, weight, stats, stats_suggestion)
}

fn new_state(step: SignInStep, data: SignInData) -> SignInState {
    SignInState { step, data }
}

fn process_asking_user_name_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let user_name = validate_user_name(input.to_lowercase());
    if user_name.len() < 3 {
        (
            state.clone(),
            if user_name.len() == 0 {
                Output::None
            } else {
                Output::Str("I'm sorry, but your name should consist of at least 3 letters.\n")
            },
        )
    } else if let Some(player) = realm.get_player_by_name(&user_name) {
        (
            new_state(
                SignInStep::AskingPassword,
                SignInData {
                    player: Some(player),
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    } else if RESERVED_NAMES.contains(&user_name) {
        (
            state.clone(),
            Output::Str("Yeah right, like I believe that...\n"),
        )
    } else {
        (
            new_state(
                SignInStep::AskingUserNameConfirmation,
                SignInData {
                    user_name,
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_user_name_confirmation_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if answer == "yes" || answer == "y" {
        (
            new_state(SignInStep::AskingSignUpPassword, state.data.clone()),
            Output::None,
        )
    } else if answer == "no" || answer == "n" {
        (
            new_state(SignInStep::SessionClosed, state.data.clone()),
            Output::Str("Okay, bye."),
        )
    } else {
        (
            state.clone(),
            Output::Str("Please answer with yes or no.\n"),
        )
    }
}

fn process_asking_sign_up_password_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    if input.len() < 6 {
        (
            state.clone(),
            Output::String(colorize(
                "Please choose a password of at least 6 characters.\n",
                Color::Red,
            )),
        )
    } else if input.to_lowercase() == state.data.user_name.to_lowercase() {
        (
            state.clone(),
            Output::String(colorize(
                "Your password and your username may not be the same.\n",
                Color::Red,
            )),
        )
    } else if input == "123456" || input == "654321" {
        (
            state.clone(),
            Output::String(colorize(
                "Sorry, that password is too simple.\n",
                Color::Red,
            )),
        )
    } else {
        (
            new_state(
                SignInStep::AskingSignUpPasswordConfirmation,
                SignInData {
                    password: input,
                    ..state.data.clone()
                },
            ),
            Output::None,
        )
    }
}

fn process_asking_sign_up_password_confirmation_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    if state.data.password == input {
        (
            new_state(SignInStep::AskingRace, state.data.clone()),
            Output::String(colorize("Password confirmed.\n", Color::Green)),
        )
    } else {
        (
            new_state(SignInStep::AskingSignUpPassword, state.data.clone()),
            Output::String(colorize("Passwords don't match.\n", Color::Red)),
        )
    }
}

fn process_asking_race_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if let Some(race) = realm.get_race_by_name(&answer) {
        (
            new_state(
                SignInStep::AskingClass,
                SignInData {
                    race: Some(race),
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                &format!("\nYou have chosen to become a {}.\n", answer),
                Color::Green,
            )),
        )
    } else if answer.starts_with("info ") {
        let race_name = answer[5..].trim();
        (
            state.clone(),
            if let Some(race) = realm.get_race_by_name(&race_name) {
                Output::String(format!(
                    "\n{}\n  {}\n",
                    highlight(&capitalize(&race_name)),
                    split_lines(&race.get_description(), 78).join("\n  ")
                ))
            } else if race_name.starts_with("<") && race_name.ends_with(">") {
                Output::Str(
                    "\nSorry, you are supposed to replace <race> with the name of an \
                    actual race. For example: *info human*.\n",
                )
            } else {
                Output::String(format!(
                    "\nI don't know anything about the \"{}\" race.\n",
                    race_name
                ))
            },
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_class_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    let classes = match &state.data.race {
        Some(race) => race
            .get_classes()
            .iter()
            .filter_map(|object_ref| realm.get_class(object_ref.get_id()))
            .collect(),
        None => Vec::new(),
    };

    if let Some(class) = classes.iter().find(|class| class.get_name() == answer) {
        (
            new_state(
                SignInStep::AskingGender,
                SignInData {
                    class: Some(class.clone()),
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                &format!("\nYou have chosen to become a {}.\n", answer),
                Color::Green,
            )),
        )
    } else if answer.starts_with("info ") {
        let class_name = answer[5..].trim();
        (
            state.clone(),
            if let Some(class) = classes.iter().find(|class| class.get_name() == class_name) {
                Output::String(format!(
                    "\n{}\n  {}\n",
                    highlight(&capitalize(&class_name)),
                    split_lines(&class.get_description(), 78).join("\n  ")
                ))
            } else if class_name.starts_with("<") && class_name.ends_with(">") {
                Output::Str(
                    "\nSorry, you are supposed to replace <class> with the name of an \
                    actual class. For example: *info knight*.\n",
                )
            } else {
                Output::String(format!(
                    "\nI don't know anything about the \"{}\" class.\n",
                    class_name
                ))
            },
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignInStep::AskingRace, state.data.clone()),
            Output::None,
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_gender_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if answer == "male" || answer == "m" {
        (
            new_state(
                SignInStep::AskingExtraStats,
                SignInData {
                    gender: Gender::Male,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize("\nYou have chosen to be male.\n", Color::Green)),
        )
    } else if answer == "female" || answer == "f" {
        (
            new_state(
                SignInStep::AskingExtraStats,
                SignInData {
                    gender: Gender::Female,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize("\nYou have chosen to be female.\n", Color::Green)),
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignInStep::AskingClass, state.data.clone()),
            Output::None,
        )
    } else {
        (state.clone(), Output::None)
    }
}

fn process_asking_extra_stats_input(
    state: &SignInState,
    _: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if answer == "info stats" {
        (
            state.clone(),
            Output::Str(
                "\n\
                Your character has several attributes, each of which will have a value assigned. \
                Collectively, we call these your character stats. Here is an overview:\n\
                \n\
                *Strength* (STR)\n\
                  Strength primarily determines the power of your physical attacks. When\n\
                  wielding a shield, it also gives a small defense power up.\n\
                \n\
                *Dexterity* (DEX)\n\
                  Dexterity determines the speed with which attacks can be dealt. It also \n\
                  improves your chances of evading enemy attacks, and the chance of success when\n\
                  fleeing.\n\
                \n\
                *Vitality* (VIT)\n\
                  Vitality primarily determines your max. health points (HP).\n\
                \n\
                *Endurance* (END)\n\
                  Endurance primarily determines your physical defense power.\n\
                \n\
                *Intelligence* (INT)\n\
                  Intelligence determines your max. magic points (MP). Barbarians cannot assign \n\
                  any points to this attribute.\n\
                \n\
                *Faith* (FAI)\n\
                  Faith determines the magical defense power. It also decreases the chance that\n\
                  a spell will fail when cast.\n",
            ),
        )
    } else if answer == "back" || answer == "b" {
        (
            new_state(SignInStep::AskingGender, state.data.clone()),
            Output::None,
        )
    } else {
        let (mut height, mut weight, mut stats, stats_suggestion) = get_base_stats(&state.data);
        let is_barbarian =
            state.data.class.as_ref().map(|class| class.get_name()) == Some("barbarian");

        let user_stats = if answer == "accept suggestion" || answer == "accept" || answer == "a" {
            stats_suggestion
        } else {
            let expected_num_attributes = if is_barbarian { 5 } else { 6 };
            if answer.split(' ').count() != expected_num_attributes {
                return (state.clone(), Output::None);
            }
            let mut attributes = answer.split(' ');
            CharacterStats::from_stats(
                cmp::max(
                    i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                    0,
                ),
                cmp::max(
                    i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                    0,
                ),
                cmp::max(
                    i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                    0,
                ),
                cmp::max(
                    i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                    0,
                ),
                if is_barbarian {
                    0
                } else {
                    cmp::max(
                        i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                        0,
                    )
                },
                cmp::max(
                    i16::from_str(attributes.next().unwrap_or_default()).unwrap_or(0),
                    0,
                ),
            )
        };

        if user_stats.total() != 9 {
            return (
                state.clone(),
                Output::String(colorize(
                    "\nThe total of attributes should be 9.\n",
                    Color::Red,
                )),
            );
        }

        stats = stats + &user_stats;
        height += (user_stats.get(CharacterStat::INTELLIGENCE) as f32)
            - (user_stats.get(CharacterStat::DEXTERITY) as f32) / 2.0;
        weight += user_stats.get(CharacterStat::STRENGTH) as f32;

        (
            new_state(
                SignInStep::AskingSignUpConfirmation,
                SignInData {
                    height,
                    weight,
                    stats,
                    ..state.data.clone()
                },
            ),
            Output::String(colorize(
                "\nYour character stats have been recorded.\n",
                Color::Green,
            )),
        )
    }
}

fn process_asking_sign_up_confirmation_input(
    state: &SignInState,
    realm: &Realm,
    input: String,
) -> (SignInState, Output) {
    let answer = input.to_lowercase();
    if realm.get_player_by_name(&state.data.user_name).is_some() {
        (
            new_state(SignInStep::SessionClosed, state.data.clone()),
            Output::Str(
                "Uh-oh, it appears someone has claimed your character name while \
                you were creating yours. I'm terribly sorry, but it appears you \
                will have to start over.\n",
            ),
        )
    } else if answer == "yes" || answer == "y" {
        /* player = Realm.createObject("Player");
        player.admin = Realm.players().isEmpty();
        player.name = signUpData.userName;
        player.race = signUpData.race;
        player.characterClass = signUpData.characterClass;
        player.gender = signUpData.gender;
        player.stats = signUpData.stats;
        player.height = signUpData.height;
        player.weight = signUpData.weight;
        player.currentRoom = signUpData.race.startingRoom;

        player.hp = player.maxHp;
        player.mp = player.maxMp;
        player.gold = 100;

        player.setPassword(signUpData.password);*/

        // TODO: logSessionEvent(this._session.source, "Character created for player " + player.name);
        (
            new_state(SignInStep::SignedIn, state.data.clone()),
            Output::None,
        )
    } else if answer == "no" || answer == "n" || answer == "back" || answer == "b" {
        (
            new_state(SignInStep::AskingExtraStats, state.data.clone()),
            Output::None,
        )
    } else {
        (
            state.clone(),
            Output::Str("Please answer with yes or no.\n"),
        )
    }
}

fn validate_user_name(input: String) -> String {
    let mut user_name = String::new();
    let mut num_special_chars = 0;
    for character in input.chars() {
        if is_letter(character) {
            user_name.push(if user_name.len() == 0 {
                character.to_ascii_uppercase()
            } else {
                character
            });
        } else if user_name.len() > 0
            && num_special_chars == 0
            && (character == '\'' || character == '-')
        {
            user_name.push(character);
            num_special_chars += 1;
        }
        if user_name.len() >= 12 {
            break;
        }
    }
    if user_name.len() > 1 && !is_letter(user_name.chars().rev().next().unwrap_or_default()) {
        user_name = user_name.chars().take(user_name.len() - 1).collect();
    }
    user_name
}
